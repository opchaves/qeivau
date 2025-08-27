config-exe:
	@cmake -S standalone -B build/standalone

build-exe:
	@cmake --build build/standalone

run-exe: build-exe
	@./build/standalone/QeiVau --filename=${DB}

config-test:
	@cmake -S test -B build/test

build-test:
	@cmake --build build/test

run-test: build-test
	@./build/test/QeiVauTests
