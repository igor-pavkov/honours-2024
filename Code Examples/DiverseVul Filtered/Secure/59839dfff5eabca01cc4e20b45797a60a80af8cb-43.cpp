void fx_init(struct kvm_vcpu *vcpu)
{
	unsigned after_mxcsr_mask;

	/*
	 * Touch the fpu the first time in non atomic context as if
	 * this is the first fpu instruction the exception handler
	 * will fire before the instruction returns and it'll have to
	 * allocate ram with GFP_KERNEL.
	 */
	if (!used_math())
		kvm_fx_save(&vcpu->arch.host_fx_image);

	/* Initialize guest FPU by resetting ours and saving into guest's */
	preempt_disable();
	kvm_fx_save(&vcpu->arch.host_fx_image);
	kvm_fx_finit();
	kvm_fx_save(&vcpu->arch.guest_fx_image);
	kvm_fx_restore(&vcpu->arch.host_fx_image);
	preempt_enable();

	vcpu->arch.cr0 |= X86_CR0_ET;
	after_mxcsr_mask = offsetof(struct i387_fxsave_struct, st_space);
	vcpu->arch.guest_fx_image.mxcsr = 0x1f80;
	memset((void *)&vcpu->arch.guest_fx_image + after_mxcsr_mask,
	       0, sizeof(struct i387_fxsave_struct) - after_mxcsr_mask);
}