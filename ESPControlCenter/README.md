# ESP Control Center

ESP Control Center is a multifunctional unit for monitoring and interacting with
smart home equipment (like a thermostat, AC units, home server etc.).

## Hardware components

- ESP12E module [[AliExpress](https://www.aliexpress.com/item/1PCS-ESP-12F-ESP-12E-upgrade-ESP8266-Remote-Serial-Port-WIFI-Wireless-Module/32714088769.html)]
- SSD1306-based OLED display [[AliExpress](https://www.aliexpress.com/item/1pcs-Free-Shipping-White-Blue-White-and-Blue-color-0-96-inch-128X64-OLED-Display-Module/32717950155.html)]
- Push buttons
- *TODO: power supply, battery management*
 - https://www.aliexpress.com/item/DD0603SA-3V3-Auto-Buck-Boost-DC-DC-0-9-6V-to-3-3V-Step-UP-Step/32787898065.html
 - https://www.aliexpress.com/item/0-7-5V-to-3V-3-3V-5V-DC-DC-Boost-Converter-voltage-Step-up-Module/32800430445.html

## Software components

### u8g2 graphics library

#### Custom fonts from TTF

1. Download `otf2bdf` from [rtfreesoft](http://rtfreesoft.blogspot.hu/2014/12/otf2bdf-self-forked.html)
1. Download `bdfconv` from [u8g2's GitHub](https://github.com/olikraus/u8g2/blob/master/tools/font/bdfconv/bdfconv.exe)
1. (optional) Download `Chortkeh BDF Font Viewer` from [Chortkeh](http://www.chortkeh.com/font-toolbox/bdf-font-viewer) if you want to
see the contents of the generated BDF file on Windows
1. Generate a BDF file from the TTF font of your choice by running this command:
`otf2bdf.exe -r 72 -p 32 "c:\Work\ultimate_oldschool_pc_font_pack_v1.0\PxPlus (TrueType - extended charset)\PxPlus_IBM_VGA8.ttf" -o c:\Work\pxplus-vga8-32.bdf`
1. (optional) Check the result with `Chortkeh BDF Font Viewer` and see the character codes you want to encode in the next step
1. Convert the BDF file to u8g2's binary format via `bdfconv.exe -v -f 1 -m "32,48-57" ..\pxplus-vga8-32.bdf -o ..\pxplus-vga8-32.c -n pxplus_vga8_32_nums -d ..\pxplus-vga8-32.bdf`
