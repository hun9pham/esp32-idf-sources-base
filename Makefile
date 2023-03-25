Print = @echo ""


# OPTION_BUILD 		?= boot_app

ESP_IDF_PATH 		= $(HOME)/sdk/esp-idf
SERIAL_PORT_OPEN 	= ttyUSB0

BASH_GREEN = '\e[0;32m'
BASH_RST   = '\e[0m'
BASH_BLUE  = '\e[0;33m'
BASH_RED   = '\e[0;31m'

ifeq ($(OPTION_BUILD),boot_app)
	TOOLS_BUILD = idf.py build
	TOOLS_FLASH = idf.py -p /dev/$(SERIAL_PORT_OPEN) flash monitor
else
	TOOLS_BUILD = idf.py app
	TOOLS_FLASH = idf.py -p /dev/$(SERIAL_PORT_OPEN) app-flash monitor
endif

.PHONY: all
all:
	$(TOOLS_BUILD)

.PHONY: flash
flash: all
	$(TOOLS_FLASH)

.PHONY: monitor
monitor:
	idf.py monitor
	
.PHONY: clean
clean:
	rm -rf build

#-===========================
# - Setup environment build -
#-===========================
.PHONY: help
help:
	$(Print) "   _______  ____    ___________  ____ ___ "
	$(Print) "  /  _/ _ \/ __/___/ __/ __/ _ \|_  /|_  |"
	$(Print) " _/ // // / _//___/ _/_\ \/ ___//_ </ __/ "
	$(Print) "/___/____/_/     /___/___/_/  /____/____/ "
                                          
	$(Print) "make [option]"
	$(Print) "Options:"
	$(Print) "  -all    : Build the Project"
	$(Print) "  -flash  : Flash onto the Device"
	$(Print) "  -config : Setup the environment variables"

.PHONY: config
config:
	$(Print) "   _______  ____    ___________  ____ ___ "
	$(Print) "  /  _/ _ \/ __/___/ __/ __/ _ \|_  /|_  |"
	$(Print) " _/ // // / _//___/ _/_\ \/ ___//_ </ __/ "
	$(Print) "/___/____/_/     /___/___/_/  /____/____/ "

	$(Print) Run following commands on working terminal: $(BASH_GREEN)
	$(Print) "------------------------------------------"
	$(Print) "1/ export IDF_PATH=${ESP_IDF_PATH}"
	$(Print) "2/  . $(ESP_IDF_PATH)/export.sh"
	$(Print) "3/ idf.py set-target esp32"
	$(Print) "------------------------------------------"
	$(Print) $(BASH_RED)Critical: All this setup will be clear when changing to other terminal.
