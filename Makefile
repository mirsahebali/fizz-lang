.PHONY: build clean tests dbg ln-lsp

ln-lsp:
	ln -s compile_commands.json .


build:
	mkdir -p build
	cd build && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. && make

dbg: build
	./build/fizzlang-debug

tests: build
	./build/fizzlang-tests

clean:
	rm -rf build deps
