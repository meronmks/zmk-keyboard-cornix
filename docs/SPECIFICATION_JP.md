# Cornix ZMK 仕様・キーマップガイド

最終確認日: 2026-08-21

この文書は、このリポジトリから生成される Cornix ファームウェアの「意図した仕様」を日本語で確認するための資料です。
実装と説明が食い違った場合は、最終的には各節に記載したソースファイルが正です。

> [!IMPORTANT]
> この文書が対象とするのは `config/cornix.keymap` を使用する `LAYOUT_50` です。
> `config/cornix42.keymap` は対象外です。

## 1. まず見る要約

### 通常の左右分割構成

- 左側: central。USB/Bluetoothの接続先と右側とのsplit接続を管理する。
- 右側: peripheral。単独ではUSBキーボードとして出力しない。
- 左右とも2個のRGB LEDを使い、LED 0が電池、LED 1が接続状態を示す。
- 左右ともファームウェア更新が必要。

### ドングル構成

- nice!nanoドングル: central。
- `cornix_ph_left//zmk`: 左側peripheral。
- `cornix_right//zmk`: 右側peripheral。
- キーボード左右のRGBは有効。現在、ドングル側のRGB indicatorは設定していない。

### キーマップの重要操作

| 操作 | 結果 |
|---|---|
| 左親指 `Esc/FN` をタップ | Esc |
| 左親指 `Esc/FN` をホールド | FNレイヤー |
| 左親指 `英数/Lower` をタップ | `LANG2`（一般に英数） |
| 左親指 `英数/Lower` をホールド | LowerまたはJIS Lowerレイヤー |
| 右親指 `かな/MouseBT` をタップ | `LANG1`（一般にかな） |
| 右親指 `かな/MouseBT` をホールド | Mouse/BTレイヤー |
| `Esc/FN` と `英数/Lower` を同時押し | JISモード切替 |
| `英数/Lower` と `かな/MouseBT` を同時ホールド | Adjustレイヤー |

`LANG1` / `LANG2` の実際のIME動作はOS側のキーボード・入力ソース設定に依存します。

## 2. 物理レイアウトと表記

有効な物理レイアウトは50キー、位置番号は0〜49です。

```text
左上段  00 01 02 03 04 05       06 07 08 09 10 11  右上段
左中段  12 13 14 15 16 17       18 19 20 21 22 23  右中段
左下段  24 25 26 27 28 29 30 31 32 33 34 35 36 37  右下段
左最下段38 39 40 41 42 43       44 45 46 47 48 49  右最下段
```

キーマップ図では次の表記を使います。

| 表記 | 意味 |
|---|---|
| `A / Layer` | タップでA、ホールドでLayer |
| `▽` | 下位の有効レイヤーへ透過する `&trans` |
| `×` | 入力を無効化し、下位にも渡さない `&none` |
| `BT0`〜`BT4` | Bluetooth profile 0〜4 |
| `MB1`〜`MB3` | マウスボタン1〜3 |

レイヤーは番号が大きいほど優先されます。`▽` は下のレイヤーの同じ位置を使用し、`×` はそこで入力を止めます。

## 3. Baseレイヤー

Cornix純正ファームウェア v1.12 の基本配列を基にした通常入力レイヤーです。

```text
TAB    Q     W     E       R          T        | Y     U          I      O     P      BSPC
CAPS   A     S     D       F          G        | H     J          K      L     \      ENTER
LSHFT  Z     X     C       V          B   × ×  | N     M          ,      .     ↑      /
LCTRL  LALT  LGUI  Esc/FN  英数/Lower  Space    | BSPC  かな/Mouse  ENTER  ←     ↓      →
```

### Hold-tap設定

| 設定 | 値 |
|---|---:|
| hold判定時間 | 250 ms |
| quick tap | 200 ms |
| flavor | hold-preferred |

250 ms以内の単押しはタップ側、それ以外や別キーとの組み合わせではホールド側を優先します。

### エンコーダー

Base、Windows、Lower、Mouse/BT、FN、JIS Lowerには次の順で定義されています。

| エンコーダー | 時計回り | 反時計回り |
|---|---|---|
| 左 | 音量アップ | 音量ダウン |
| 右 | Page Up | Page Down |

Adjust、Navi、Debug、JIS Mode overlayには個別の `sensor-bindings` がありません。
エンコーダー動作に差が出た場合は、まず該当レイヤーの定義を確認してください。

## 4. コンボ

コンボは指定した2キーをほぼ同時に押したときに発動します。

| 物理位置（Baseの文字） | 出力 | 有効範囲 |
|---|---|---|
| `W` + `R` | Esc | Base / Windows |
| `S` + `F` | Tab | Base / Windows |
| `J` + `L` | Enter | Base / Windows |
| `Esc/FN` + `英数/Lower` | JISモード切替 | 全体、timeout 50 ms |

最後のJISコンボは左側だけで完結するため、左右間通信によるcombo遅延を避けています。

## 5. レイヤー一覧

| # | 名前 | 主な用途 | 入り方 | 現在の状態 |
|---:|---|---|---|---|
| 0 | Base | 通常入力 | 常時 | 使用中 |
| 1 | Windows | Windows向け差し替え予約 | Adjustからtoggle | 全キー透過、実質変化なし |
| 2 | Lower | 数字・記号 | `英数/Lower`をホールド | 使用中 |
| 3 | Mouse/BT | マウス・Bluetooth | `かな/Mouse`をホールド | 使用中 |
| 4 | Adjust | profile管理・設定 | LowerとMouse/BTを同時ホールド | 使用中 |
| 5 | Navi | マウス・スクロール・カーソル | 現在は入口なし | 休眠中 |
| 6 | FN | F1〜F12・ナビゲーション | `Esc/FN`をホールド | 使用中 |
| 7 | Debug | bond削除・bootloader | 現在は入口なし | 休眠中 |
| 8 | JIS Lower | JIS OS向け数字・記号 | JISモード中に`英数/Lower`をホールド | 使用中 |
| 9 | JIS Mode | JIS補正overlay | JIS toggle操作 | 使用中 |

NaviとDebugは定義されていますが、現在のBaseから到達するキーはありません。「定義があるので使える」とは限らない点に注意してください。

## 6. Lowerレイヤー

Base上の次の位置が変化します。記載のない `▽` はBaseへ透過します。

```text
ESC  1  2  3  4  5  | 6  7  8  9  0  ▽
 ×   ×  `  ×  ;  -  | =  '  [  ]  ▽  ▽
 ▽   ×  ×  ×  ×  ×  | ×  ×  ▽  ▽  ×  ▽
 ▽   ▽  ▽  ▽  ▽  ▽  | ▽  ▽  ▽  ×  ×  INT1
```

右端の `INT1` はJISキーボードの「ろ」位置で、JIS配列OSでは `\`、Shift併用で `_` を出すために使います。

## 7. JISモード

### 目的

ZMKはUS HID keycodeを送ります。ホストOSをJISキーボードとして設定していると、一部記号の位置がUS配列と一致しません。
JISモードは、通常文字を変えずに記号だけをJIS OS向けに補正します。

JISモードは次のどの操作でも切り替えられます。

- `Esc/FN` + `英数/Lower` のコンボ。
- Mouse/BTレイヤーの `I` 位置。
- Adjustレイヤーの `K` 位置。
- JIS Lowerレイヤーの `CAPS` 位置。

### JISモードで変わること

- Baseの `\` 位置は、通常 `¥`、Shift時 `|` になる。
- `英数/Lower` のホールド先がLayer 2 LowerからLayer 8 JIS Lowerへ変わる。
- アルファベット、数字の単押し、修飾キーはBaseを透過する。

### JIS Lowerの意図した出力

次の表は、ホストOSがJIS配列設定の場合の出力です。

| キー表示 | 単押し | Shift併用 |
|---|---|---|
| `1` | `1` | `!` |
| `2` | `2` | `@` |
| `3` | `3` | `#` |
| `4` | `4` | `$` |
| `5` | `5` | `%` |
| `6` | `6` | `^` |
| `7` | `7` | `&` |
| `8` | `8` | `*` |
| `9` | `9` | `(` |
| `0` | `0` | `)` |
| `` ` ``位置 | `` ` `` | `~` |
| `;`位置 | `;` | `:` |
| `-`位置 | `-` | `_` |
| `=`位置 | `=` | `+` |
| `'`位置 | `'` | `"` |
| `[`位置 | `[` | `{` |
| `]`位置 | `]` | `}` |
| `INT1` | `\` | `_` |

> [!WARNING]
> ホストOSがUS配列なのにJISモードを有効にすると、意図しない記号になります。
> 「文字がおかしい」ときは、最初にOSの配列設定とJISモードの状態が一致しているか確認してください。

## 8. Mouse/BTレイヤー

表の「Base位置」は、キーを見つけやすくするためBaseでの刻印・出力名を使っています。

| Base位置 | 動作 |
|---|---|
| `TAB` | 現在選択中のBT profileのbondを削除 `BT_CLR` |
| `CAPS` | BT profile 0を選択 |
| `LSHFT` | BT profile 1を選択 |
| `LCTRL` | BT profile 2を選択 |
| `LALT` | USB/Bluetooth出力をtoggle |
| `F` | Mouse Button 1 |
| `V` | Mouse Button 2 |
| `I` | JISモードtoggle |
| `P` | Print Screen |
| `BSPC`（上段右端） | Delete |
| `H` / `J` / `K` / `L` | マウス 左 / 下 / 上 / 右 |
| `ENTER`（中段右端） | Insert |

profile 3と4の選択・切断はAdjustレイヤーから行います。

## 9. Adjustレイヤー

`英数/Lower`と`かな/Mouse`を同時にホールドすると、Lower + Mouse/BTのconditional layerとして有効になります。

| Base位置 | 動作 |
|---|---|
| `TAB` | 現在選択中のBT profileのbondを削除 `BT_CLR` |
| `Q`〜`T` | BT profile 0〜4を選択 |
| `A`〜`G` | BT profile 0〜4を切断 |
| `LSHFT` | 全Bluetooth bond削除 `BT_CLR_ALL` |
| `H` | Windows layer toggle |
| `J` | Baseへ戻る `to 0` |
| `K` | JISモードtoggle |
| `ENTER`（中段右端） | Windows layer toggle |
| `↑`位置 | RGBを含む外部電源をON |
| `/`位置 | Caps Lock |

`BT_CLR` / `BT_CLR_ALL` 後はホスト側でもCornixのBluetooth登録を削除し、再ペアリングしてください。
これらのキーは左右split間のbondを削除するものではありません。

## 10. FNレイヤー

```text
F1     F2    F3    F4   F5   F6  | F7     F8     F9     F10  F11  F12
 ×      ×     ×     ×    ×    ×   | Home   PgDn   PgUp   End   ×    ×
LSHFT   ×     ×     ×    ×    ×   |  ×      ×      ×      ×    ↑    ×
LCTRL  LALT  LGUI   ▽    ▽    ▽   | RSHFT  RCTRL  RALT    ←    ↓    →
```

## 11. Navi / Debug / Windowsレイヤー

### Windows

現在は全キーが `▽` です。toggleしてもBaseと同じ動作です。将来のWindows専用差し替え用として予約されています。

### Navi

マウス移動、スクロール、カーソル、Mouse Button 1〜3が定義されていますが、現在はこのレイヤーへ入るキーがありません。

### Debug

- 上段左側に `BT_CLR` とBT profile 0〜4選択。
- 左下段の `B` 位置と右下段の `N` 位置に、それぞれのhalfをbootloaderへ入れる動作。
- 現在はこのレイヤーへ入るキーがないため、通常操作からは使用できない。

## 12. RGBインジケーター

### このリポジトリで固定している仕様

| 項目 | 設定 |
|---|---|
| LED | 各halfにWS2812を2個 |
| LED 0 | 電池状態 |
| LED 1 | 接続状態 |
| 明るさ | 64 / 255（約25%） |
| spatial mapping | 有効 |
| Caps Lock表示 | 無効 |
| layer表示 | 無効 |
| idle時の外部電源OFF | indicatorが消えてから1000 ms |

狙いはCornix純正ファームウェアと同じく「電池」と「接続」を2灯の主用途にすることです。
RMK純正ファームウェアのアニメーションを完全に同一のタイミングで再現する、という意味ではありません。

### 現在の `zmk-rgbled-widget` が提供する表示

色やアニメーションの一部は外部moduleの既定値を継承します。

#### LED 0: 電池

| 状態 | 目安となる表示 |
|---|---|
| 80%より上 | 緑 |
| 20〜80% | 黄 |
| 20%未満 | 赤点滅 |
| 5%未満 | 赤の高速点滅 |
| 充電中 | 緑のpulse |
| peripheral不明・切断 | magenta |

#### LED 1: 接続

| 状態 | 目安となる表示 |
|---|---|
| BT profile 0 / 1 / 2 / 3 / 4 | 緑 / 赤 / 青 / 黄 / magenta |
| advertising | 黄のpulse |
| 接続切断 | 赤点滅 |
| split peripheral接続 | 青 |
| split peripheral切断 | 赤点滅 |

> [!NOTE]
> `config/west.yml` は `zmk-rgbled-widget` の `main` を参照しており、commitを固定していません。
> module更新によって色やanimationの既定値が変わる可能性があります。
> LED番号、明るさ、Caps Lock無効、電源OFF時間はこのリポジトリ側で明示しています。

### RGB配線

| half | RGB電源制御 | WS2812 data |
|---|---|---|
| 左 | P0.13 | P0.24 / SPI3 MOSI |
| 右 | P0.24 | P0.13 / SPI3 MOSI |

indicatorが消灯してもRGB電源がすぐ切れない場合、1000 msのidle timeout内である可能性があります。
何らかのstatic indicatorが点灯中なら、timeoutでは電源を切りません。

## 13. ビルド成果物とflash対象

| artifact | board / shield | 用途 |
|---|---|---|
| `cornix_left_default_nosd` | `cornix_left//zmk` + `cornix_indicator` | 通常構成の左central |
| `cornix_left_for_dongle_nosd` | `cornix_ph_left//zmk` + `cornix_indicator` | ドングル構成の左peripheral |
| `cornix_right_nosd` | `cornix_right//zmk` + `cornix_indicator` | 右peripheral、両構成共通 |
| `cornix_dongle_nosd` | `nice_nano//zmk` + dongle shields | ドングルcentral |
| `cornix_reset` | `cornix_right//zmk` + `settings_reset` | Cornix側設定reset |
| `reset_nicenano_nosd` | `nice_nano//zmk` + `settings_reset` | ドングル設定reset |

通常構成では `cornix_left_default_nosd` と `cornix_right_nosd` を使います。
ドングル構成では `cornix_dongle_nosd`、`cornix_left_for_dongle_nosd`、`cornix_right_nosd` を使います。

Cornixはno-SoftDevice flash layoutを前提にしています。左右・ドングルで役割の異なるUF2を取り違えないでください。

## 14. 「想定と違う」ときの確認順

1. 通常構成かドングル構成かを確認する。
2. 左右両方に今回生成したUF2をflashしたか確認する。
3. OSの物理キーボード配列設定とJISモードが一致しているか確認する。
4. JISモードが意図せずtoggleされていないか確認する。
5. `&trans` と `&none` の違いを確認する。`&none` はBaseにも戻らない。
6. Navi / Debugは定義だけで入口がないことを確認する。
7. RGBの場合、`cornix_indicator`付きartifactか、左右とも更新済みかを確認する。
8. 外部moduleが`main`更新で変化していないか確認する。
9. bondやroleを変更した場合はsettings resetと再ペアリングを検討する。

## 15. 実装との照合先

| 確認したい内容 | ソースファイル |
|---|---|
| キー、レイヤー、コンボ、JIS mode | `config/cornix.keymap` |
| JIS keycode変換 | `config/includes/jis_keys.h` |
| 物理位置の別名 | `config/includes/cornix54.h` |
| build対象とartifact名 | `build.yaml` |
| 外部moduleの取得先 | `config/west.yml` |
| RGBの固定設定 | `boards/shields/cornix_indicator/cornix_indicator.conf` |
| RGBを有効にするoverlay | `boards/shields/cornix_indicator/cornix_indicator.overlay` |
| 左RGB pin | `boards/jzf/cornix/cornix_left_common.dtsi` |
| 右RGB pin | `boards/jzf/cornix/cornix_right.dts` |
| 物理50キーレイアウト | `boards/jzf/cornix/cornix-layouts.dtsi` |
| encoder hardware | `boards/jzf/cornix/cornix_sensors.dtsi` |

### 変更時の同期チェック

次を変更したときは、この文書も同じcommitで更新してください。

- `config/cornix.keymap` のレイヤー番号、キー、コンボ。
- `build.yaml` のboard、shield、artifact名。
- `cornix_indicator.conf` の色・明るさ・LED mapping・timeout。
- `config/west.yml` のZMKまたはRGB module revision。
- RGB配線や物理レイアウトのdevicetree。

## 参考リンク

- [ZMK Keymaps & Behaviors](https://zmk.dev/docs/keymaps)
- [ZMK Keymap Configuration](https://zmk.dev/docs/config/keymap)
- [ZMK Encoder Configuration](https://zmk.dev/docs/config/encoders)
- [Cornix用 zmk-rgbled-widget fork](https://github.com/hitsmaxft/zmk-rgbled-widget)
