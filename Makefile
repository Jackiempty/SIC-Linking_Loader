BUILD_DIR = build
BUILD_TYPE := Debug

all: format
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. -GNinja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cd $(BUILD_DIR) && cmake --build .
	@-mv $(BUILD_DIR)/compile_commands.json .

test: build/bin/SICLLR
	./build/bin/SICLLR test/objf1 test/objf2

format:
	find . -name build -type d \! -prune -o -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i

clean:
	rm -rf $(BUILD_DIR)
