/*
 * Keep Cornix LED 0 usable as a dim power-on and battery-level indicator.
 *
 * zmk-rgbled-widget normally returns the battery LED to its idle color after
 * the startup indication. This listener restores the appropriate persistent
 * battery indication whenever the local battery, USB, or activity state
 * changes. This also restores LED 0 after ZMK enters ZMK_ACTIVITY_IDLE.
 * Charging is reapplied as a battery-level-colored blink in both ACTIVE and
 * IDLE states, while the widget retains control of the critical-battery
 * warning while active.
 * ZMK_ACTIVITY_SLEEP is intentionally left dark so deep sleep can still power
 * down the RGB rail.
 */

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/battery.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>

#include <zmk_rgbled_widget/widget.h>

LOG_MODULE_REGISTER(cornix_persistent_battery, CONFIG_ZMK_LOG_LEVEL);

#define INDICATOR_UPDATE_DELAY_MS 100
#define INDICATOR_IDLE_RESTORE_DELAY_MS 10
#define INDICATOR_CHARGING_BLINK_HALF_PERIOD_MS 500

static struct k_work_delayable indicator_update_work;
static enum zmk_activity_state indicator_activity_state = ZMK_ACTIVITY_ACTIVE;
static bool charging_blink_on;

static uint8_t battery_color(uint8_t level) {
    if (level == 0) {
        return CONFIG_RGBLED_WIDGET_BATTERY_COLOR_MISSING;
    }

    if (level >= CONFIG_RGBLED_WIDGET_BATTERY_LEVEL_HIGH) {
        return CONFIG_RGBLED_WIDGET_BATTERY_COLOR_HIGH;
    }

    if (level >= CONFIG_RGBLED_WIDGET_BATTERY_LEVEL_LOW) {
        return CONFIG_RGBLED_WIDGET_BATTERY_COLOR_MEDIUM;
    }

    return CONFIG_RGBLED_WIDGET_BATTERY_COLOR_LOW;
}

static void update_indicator(struct k_work *work) {
    ARG_UNUSED(work);

    if (indicator_activity_state == ZMK_ACTIVITY_SLEEP) {
        return;
    }

    const uint8_t level = zmk_battery_state_of_charge();

    /* Keep blinking in the current battery-level color after entering IDLE. */
    if (zmk_usb_is_powered() && level < 99) {
        charging_blink_on = !charging_blink_on;

        int ret = ws2812_clear_status_led(STATUS_BATTERY);
        if (ret < 0) {
            LOG_WRN("Could not update charging animation: %d", ret);
        } else if (charging_blink_on) {
            ret = ws2812_set_status_led(STATUS_BATTERY, battery_color(level), 0, true);
            if (ret < 0) {
                LOG_WRN("Could not set charging animation color: %d", ret);
            }
        }

        k_work_reschedule(&indicator_update_work,
                          K_MSEC(INDICATOR_CHARGING_BLINK_HALF_PERIOD_MS));
        return;
    }

    charging_blink_on = false;

    /* Preserve the widget's critical-battery warning while active. */
    if (indicator_activity_state == ZMK_ACTIVITY_ACTIVE && level > 0 &&
        level <= CONFIG_RGBLED_WIDGET_BATTERY_LEVEL_CRITICAL) {
        return;
    }

    int ret = ws2812_clear_status_led(STATUS_BATTERY);
    if (ret < 0) {
        LOG_WRN("Could not stop battery animation: %d", ret);
        return;
    }

    ret = ws2812_set_status_led(STATUS_BATTERY, battery_color(level), 0, true);
    if (ret < 0) {
        LOG_WRN("Could not set persistent battery indicator: %d", ret);
    }
}

static int indicator_state_changed(const zmk_event_t *eh) {
    ARG_UNUSED(eh);

    k_work_reschedule(&indicator_update_work, K_MSEC(INDICATOR_UPDATE_DELAY_MS));
    return ZMK_EV_EVENT_BUBBLE;
}

static int indicator_activity_changed(const zmk_event_t *eh) {
    const struct zmk_activity_state_changed *event = as_zmk_activity_state_changed(eh);
    if (event == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    indicator_activity_state = event->state;

    if (event->state == ZMK_ACTIVITY_SLEEP) {
        k_work_cancel_delayable(&indicator_update_work);
        charging_blink_on = false;
    } else {
        /* Run after zmk-rgbled-widget has handled the same activity event. */
        k_work_reschedule(&indicator_update_work,
                          K_MSEC(INDICATOR_IDLE_RESTORE_DELAY_MS));
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(cornix_persistent_battery_listener, indicator_state_changed);
ZMK_SUBSCRIPTION(cornix_persistent_battery_listener, zmk_battery_state_changed);
ZMK_SUBSCRIPTION(cornix_persistent_battery_listener, zmk_usb_conn_state_changed);

ZMK_LISTENER(cornix_persistent_battery_activity_listener, indicator_activity_changed);
ZMK_SUBSCRIPTION(cornix_persistent_battery_activity_listener, zmk_activity_state_changed);

static int cornix_persistent_battery_init(void) {
    k_work_init_delayable(&indicator_update_work, update_indicator);
    k_work_schedule(&indicator_update_work,
                    K_MSEC(CONFIG_CORNIX_PERSISTENT_BATTERY_STARTUP_DELAY_MS));
    return 0;
}

SYS_INIT(cornix_persistent_battery_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
