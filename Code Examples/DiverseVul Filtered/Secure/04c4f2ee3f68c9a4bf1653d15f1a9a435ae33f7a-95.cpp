static void vmx_reset_x2apic_msrs(struct kvm_vcpu *vcpu, u8 mode)
{
	unsigned long *msr_bitmap = to_vmx(vcpu)->vmcs01.msr_bitmap;
	unsigned long read_intercept;
	int msr;

	read_intercept = (mode & MSR_BITMAP_MODE_X2APIC_APICV) ? 0 : ~0;

	for (msr = 0x800; msr <= 0x8ff; msr += BITS_PER_LONG) {
		unsigned int read_idx = msr / BITS_PER_LONG;
		unsigned int write_idx = read_idx + (0x800 / sizeof(long));

		msr_bitmap[read_idx] = read_intercept;
		msr_bitmap[write_idx] = ~0ul;
	}
}