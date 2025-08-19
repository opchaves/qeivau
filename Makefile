config-exe:
	@cmake -S standalone -B build/standalone

build-exe:
	@cmake --build build/standalone

run-exe: build-exe
	@./build/standalone/QeiVau
