cc=clang

vm=./bin/set
vm_src=./src/set.c

caasm=./bin/caasmbler
caasm_src=./src/basic_assembler.c

decca=./bin/decca
decca_src=./src/decca.c

tester=./src/tester.c

caasm: 
	$(cc) -o $(caasm) $(caasm_src) -g -fsanitize=address -Wall -Wextra

caasm_gdb: 
	$(cc) -o $(caasm) $(caasm_src) -g -Wall -Wextra

vm_normal: 
	$(cc) -o $(vm) $(vm_src) -m32

vm_debug:
	$(cc) -o $(vm) $(vm_src) -g -fsanitize=address

decca: 
	$(cc) -o $(decca) $(decca_src) -g -fsanitize=address

view: decca
	$(decca) ./out.caic

asm: caasm
	$(caasm) ./example.caasm

asm_debug: caasm_gdb
	$(cassm) ./example.caasm

vm: vm_debug
	$(vm) ./out.caic

run:
	$(caasm) ./example.caasm && $(vm) ./out.caic

testall: $(tester)
	chmod +x $(tester) && ./$(tester)

args: 
	clang ./src/cliargs.c -o ./bin/temp -g -fsanitize=address
	
argsrun: args
	./bin/temp
	
test: 
	clang $(tester) -o ./bin/tester -g -fsanitize=address
	./bin/tester

all: decca caasm vm_normal
