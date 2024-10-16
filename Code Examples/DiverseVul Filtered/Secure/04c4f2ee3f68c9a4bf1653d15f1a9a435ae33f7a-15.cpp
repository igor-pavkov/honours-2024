noinline void invept_error(unsigned long ext, u64 eptp, gpa_t gpa)
{
	vmx_insn_failed("kvm: invept failed: ext=0x%lx eptp=%llx gpa=0x%llx\n",
			ext, eptp, gpa);
}