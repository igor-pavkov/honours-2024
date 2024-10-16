static int handle_apic_access(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	u64 exit_qualification;
	enum emulation_result er;
	unsigned long offset;

	exit_qualification = vmcs_read64(EXIT_QUALIFICATION);
	offset = exit_qualification & 0xffful;

	er = emulate_instruction(vcpu, kvm_run, 0, 0, 0);

	if (er !=  EMULATE_DONE) {
		printk(KERN_ERR
		       "Fail to handle apic access vmexit! Offset is 0x%lx\n",
		       offset);
		return -ENOTSUPP;
	}
	return 1;
}