static int handle_apic_write(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification = vmx_get_exit_qual(vcpu);
	u32 offset = exit_qualification & 0xfff;

	/* APIC-write VM exit is trap-like and thus no need to adjust IP */
	kvm_apic_write_nodecode(vcpu, offset);
	return 1;
}