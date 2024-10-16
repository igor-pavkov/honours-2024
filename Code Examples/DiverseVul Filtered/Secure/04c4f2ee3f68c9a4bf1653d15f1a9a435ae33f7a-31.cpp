noinline void invvpid_error(unsigned long ext, u16 vpid, gva_t gva)
{
	vmx_insn_failed("kvm: invvpid failed: ext=0x%lx vpid=%u gva=0x%lx\n",
			ext, vpid, gva);
}