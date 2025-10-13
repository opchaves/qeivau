config-exe:
	@cmake -S standalone -B build/standalone

build-exe:
	@cmake --build build/standalone

run: build-exe
	@./build/standalone/QeiVau --filename=${DB}

config-test:
	@cmake -S test -B build/test

build-test:
	@CTEST_OUTPUT_ON_FAILURE=1 cmake --build build/test

test: build-test
	@./build/test/QeiVauTests
