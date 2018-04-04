BOARD ?= nodemcuv2
FLASH_DEF ?= 4M1M
UPLOAD_PORT ?= /dev/ttyUSB0

-include makeEspArduino.mk

HOST ?= www.google.com
CERTFILE ?= $(HOST).cert

fingerprint $(CERTFILE):
	echo $(CERTFILE)
	echo -n | openssl s_client -connect $(HOST):443 | sed -ne '/-BEGIN CERTIFICATE-/,/-END CERTIFICATE-/p' > $(CERTFILE)
	openssl x509 -noout -in $(CERTFILE) -fingerprint -sha1
	openssl x509 -noout -in $(CERTFILE) -fingerprint -sha256

certificates.h: $(CERTFILE)
	mkdir -p certificates
	openssl x509 -in $(CERTFILE) -out certificates/esp8266.bin.crt -outform DER
	# openssl rsa -in esp8266.key -out certificates/esp8266.bin.key -outform DER
	xxd -i certificates/esp8266.* > certificates.h

monitor:
	minicom -b $(UPLOAD_SPEED) -D $(UPLOAD_PORT)

clean: project_clean

all: certificates.h

project_clean:
	rm -fr certificates certificates.h $(CERTFILE)

.PHONY: fingerprint monitor
