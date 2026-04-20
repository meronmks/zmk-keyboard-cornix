/*
 * JIS keyboard key aliases for ZMK firmware
 *
 * Use these macros when the host OS is configured for Japanese (JIS) keyboard layout.
 * Each alias maps to the HID keycode that produces the corresponding JIS character,
 * which differs from the US layout mapping.
 *
 * Reference: JIS X 6002 keyboard standard
 *
 * Example: JA_AT sends LBKT (US [). When OS is in JIS mode, LBKT position = @ key.
 */

#pragma once

// ─── JIS-specific input method keys ─────────────────────────────────────────

#define JA_ZKHK  GRAVE       // 半角/全角漢字 (Hankaku/Zenkaku toggle)
#define JA_YEN   INT3        // ¥ (Yen sign key)
#define JA_RO    INT1        // ろ (ro / backslash key, bottom-right of JIS)
#define JA_KANA  INT2        // カタカナ/ひらがな (Katakana/Hiragana)
#define JA_HENK  INT4        // 変換 (Henkan / Convert)
#define JA_MHEN  INT5        // 無変換 (Muhenkan / Non-convert)

// ─── Symbols that differ from US layout ──────────────────────────────────────
//
// Layout reference (JIS position → HID code to send → JIS output):
//
//  @ key  (= US [ position) → LBKT  → @    |  Shift → ` (backtick)
//  [ key  (= US ] position) → RBKT  → [    |  Shift → {
//  ] key  (= US \ position) → BSLH  → ]    |  Shift → }
//  ^ key  (= US = position) → EQUAL → ^    |  Shift → ~
//  ¥ key  (= US \ top-row)  → INT3  → ¥   |  Shift → |
//  : key  (= US ' position) → SQT   → :    |  Shift → *

#define JA_AT    LBKT        // @
#define JA_GRV   LS(LBKT)   // ` (backtick)

#define JA_LBRC  RBKT        // [
#define JA_LCBR  LS(RBKT)   // {

#define JA_RBRC  BSLH        // ]
#define JA_RCBR  LS(BSLH)   // }

#define JA_BSLS  INT1        // \ (backslash — JIS RO key)
#define JA_UNDS  LS(INT1)   // _

#define JA_CIRC  EQUAL       // ^
#define JA_TILD  LS(EQUAL)  // ~

#define JA_EQL   LS(MINUS)  // =  (JIS Shift+-)
#define JA_PLUS  LS(SEMI)   // +  (JIS Shift+;)

#define JA_COLN  SQT         // :  (JIS colon key = US apostrophe position)
#define JA_ASTR  LS(SQT)    // *  (JIS Shift+:)

#define JA_PIPE  LS(INT3)   // |  (JIS Shift+¥)

// Number-row symbols where JIS Shift differs from US Shift
//  JIS Shift+2 = "   (US Shift+2 = @)
//  JIS Shift+6 = &   (US Shift+6 = ^)
//  JIS Shift+7 = '   (US Shift+7 = &)
//  JIS Shift+8 = (   (US Shift+8 = *)
//  JIS Shift+9 = )   (US Shift+9 = ()

#define JA_DQT   LS(N2)     // "
#define JA_AMPR  LS(N6)     // &
#define JA_QUOT  LS(N7)     // '
#define JA_LPRN  LS(N8)     // (
#define JA_RPRN  LS(N9)     // )
