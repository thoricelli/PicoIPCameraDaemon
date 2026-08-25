NDK_PATH  ?= $(ANDROID_NDK_ROOT)
ABI       ?= arm64-v8a
API_LEVEL ?= 29
BUILD_DIR  = build/$(ABI)

.PHONY: all config build typescript clean

all: config build

config:
	@echo "Configuring CMake..."
	@$(CMAKE) -E make_directory $(BUILD_DIR)
	@cmake -G "Ninja" -S . -B $(BUILD_DIR) -DCMAKE_TOOLCHAIN_FILE=$(NDK_PATH)/build/cmake/android.toolchain.cmake -DANDROID_ABI=$(ABI) -DANDROID_PLATFORM=android-$(API_LEVEL) -DCMAKE_BUILD_TYPE=Release

build: typescript
	@echo "Building binary."
	@cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	@cmake --build $(BUILD_DIR) --clean-first

debug: config typescript
	@echo "Building debug binary."
	@cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	@cmake --build $(BUILD_DIR) --clean-first

typescript:
	npx -y esbuild html/script.ts --bundle --outfile=html/dist/script.js

clean:
	@echo "Cleaning build directories..."
	@rm -rf build_*
