#!/bin/sh

# $HOME/.arduino15/packages/esp8266/tools/esptool/0.4.9/esptool -vv -cd ck -cb 115200 -cp /dev/ttyUSB0 -ca 0x00000 -cf firmware.0.9.bin
$HOME/.arduino15/packages/esp8266/tools/esptool/0.4.9/esptool -vv -cd nodemcu -cb 115200 -cp /dev/ttyUSB0 -ca 0x00000 -cf firmware.0.9.bin
