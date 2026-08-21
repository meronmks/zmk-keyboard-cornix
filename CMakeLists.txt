if(CONFIG_CORNIX_PERSISTENT_BATTERY_INDICATOR)
  zephyr_library()
  zephyr_library_sources(src/cornix_persistent_battery_indicator.c)
endif()
