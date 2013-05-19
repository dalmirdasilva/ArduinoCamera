ARDUINO_LIB_PATH=/usr/share/arduino/libraries
LIB_LIST=Camera CameraAL422B CameraOV7670 CameraVC0706
SOURCE_PATH=`pwd`

all: 
	@echo "Use [install], [unistall] or [doc]"

install:
	@echo "Instaling all libraries..."
	@for lib in $(LIB_LIST) ; do \
		ln -s  $(SOURCE_PATH)/$$lib $(ARDUINO_LIB_PATH); \
	done
	@echo "done."
	
uninstall:
	@echo "Uninstaling all libraries..."
	@for lib in $(LIB_LIST) ; do \
		rm -r $(ARDUINO_LIB_PATH)/$$lib ; \
	done
	@echo "done."
		
doc:
	@echo "Running doxygen..."
	@rm -rf doc
	@mkdir doc
	doxygen doxygen.conf
	@echo "done."
	
	
