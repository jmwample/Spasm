
all: spasm

new: clean all


sub_vm: sub_vm_test.c sub_vm.c sub_vm.h
	$(CC) $^ -o $@

process: sub_vm.h process.c sub_vm.c
	$(CC) $^ -o $@

spasm: spasm_test.c spasm.c spasm.h
	$(CC) $^ -o $@

clean:
	$(RM) sub_vm test spasm process
