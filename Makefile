NDK_PATH  ?= $(ANDROID_NDK_ROOT)
ABI       ?= arm64-v8a
API_LEVEL ?= 29
BUILD_DIR  = build/$(ABI)

.PHONY: all config build clean


all: config build


config:
	@echo "Configuring CMake..."
	@mkdir -p $(BUILD_DIR)
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_TOOLCHAIN_FILE=$(NDK_PATH)/build/cmake/android.toolchain.cmake -DANDROID_ABI=$(ABI) -DANDROID_PLATFORM=android-$(API_LEVEL) -DCMAKE_BUILD_TYPE=Release

build:
	@echo "Building binary..."
	# Rebuild because for some reason, build doesn't detect changes properly!
	@cmake --build $(BUILD_DIR) --target clean
	@cmake --build $(BUILD_DIR)

clean:
	@echo "Cleaning build directories..."
	@rm -rf build_*
