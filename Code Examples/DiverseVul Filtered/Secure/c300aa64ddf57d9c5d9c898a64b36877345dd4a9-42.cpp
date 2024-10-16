static int emulator_set_msr(struct x86_emulate_ctxt *ctxt,
			    u32 msr_index, u64 data)
{
	struct msr_data msr;

	msr.data = data;
	msr.index = msr_index;
	msr.host_initiated = false;
	return kvm_set_msr(emul_to_vcpu(ctxt), &msr);
}