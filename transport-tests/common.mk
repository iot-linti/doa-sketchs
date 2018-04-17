BOARD ?= nodemcuv2
FLASH_DEF ?= 4M1M
UPLOAD_PORT ?= /dev/ttyUSB0

monitor:
	minicom -b $(UPLOAD_SPEED) -D $(UPLOAD_PORT)

.PHONY: monitor
