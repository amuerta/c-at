cc=clang

vm=./bin/set
vm_src=./src/vm/vm.c

caasm=./bin/caasmbler
caasm_src=./src/assembler/basic_assembler.c

tester=./src/testing/tester.c

caasm_bin: 
	$(cc) -o $(caasm) $(caasm_src) -g -fsanitize=address -Wall -Wextra

vm_bin:
	$(cc) -o $(vm) $(vm_src) -ggdb -fsanitize=address

run:
	$(caasm) ./example.caasm && $(vm) ./out.caic

test: 
	clang $(tester) -o ./bin/tester -g -fsanitize=address
	./bin/tester

all: decca caasm vm_normal
