# This MUST be used on a Linux system (though that includes WSL). Sorry
# Also install-deps only works on Debian systems.

# Project settings for main game card file
OBJNAME :=			fight-key
SRC :=				$(wildcard src/*.cpp)
HEADERS :=			$(wildcard include/*.hpp)

# Helper targets
.PHONY : all
all : /tmp/pico-sdk $(OBJNAME).uf2

.PHONY : install-deps
install-deps :
	apt install -y \
		git cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

/tmp/pico-sdk :
	cd /tmp; git clone -b master https://github.com/raspberrypi/pico-sdk.git
	cd /tmp/pico-sdk; git submodule update --init

.PHONY : clean
clean :
	rm -rf /tmp/pico-sdk
	rm -rf build
	rm -rf *.uf2

# Main targets
$(OBJNAME).uf2 : /tmp/pico-sdk $(SRC) $(HEADERS) CMakeLists.txt
	mkdir -p build
	cd build; PICO_SDK_PATH=/tmp/pico-sdk cmake ..
	cd build; make
	cp build/$(OBJNAME).uf2 .
