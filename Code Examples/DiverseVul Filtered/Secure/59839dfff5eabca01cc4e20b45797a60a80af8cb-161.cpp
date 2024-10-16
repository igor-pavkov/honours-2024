void kvm_report_emulation_failure(struct kvm_vcpu *vcpu, const char *context)
{
	u8 opcodes[4];
	unsigned long rip = kvm_rip_read(vcpu);
	unsigned long rip_linear;

	if (!printk_ratelimit())
		return;

	rip_linear = rip + get_segment_base(vcpu, VCPU_SREG_CS);

	kvm_read_guest_virt(rip_linear, (void *)opcodes, 4, vcpu);

	printk(KERN_ERR "emulation failed (%s) rip %lx %02x %02x %02x %02x\n",
	       context, rip, opcodes[0], opcodes[1], opcodes[2], opcodes[3]);
}