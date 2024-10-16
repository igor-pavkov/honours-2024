static noinstr void vmx_vcpu_enter_exit(struct kvm_vcpu *vcpu,
					struct vcpu_vmx *vmx)
{
	/*
	 * VMENTER enables interrupts (host state), but the kernel state is
	 * interrupts disabled when this is invoked. Also tell RCU about
	 * it. This is the same logic as for exit_to_user_mode().
	 *
	 * This ensures that e.g. latency analysis on the host observes
	 * guest mode as interrupt enabled.
	 *
	 * guest_enter_irqoff() informs context tracking about the
	 * transition to guest mode and if enabled adjusts RCU state
	 * accordingly.
	 */
	instrumentation_begin();
	trace_hardirqs_on_prepare();
	lockdep_hardirqs_on_prepare(CALLER_ADDR0);
	instrumentation_end();

	guest_enter_irqoff();
	lockdep_hardirqs_on(CALLER_ADDR0);

	/* L1D Flush includes CPU buffer clear to mitigate MDS */
	if (static_branch_unlikely(&vmx_l1d_should_flush))
		vmx_l1d_flush(vcpu);
	else if (static_branch_unlikely(&mds_user_clear))
		mds_clear_cpu_buffers();

	if (vcpu->arch.cr2 != native_read_cr2())
		native_write_cr2(vcpu->arch.cr2);

	vmx->fail = __vmx_vcpu_run(vmx, (unsigned long *)&vcpu->arch.regs,
				   vmx->loaded_vmcs->launched);

	vcpu->arch.cr2 = native_read_cr2();

	/*
	 * VMEXIT disables interrupts (host state), but tracing and lockdep
	 * have them in state 'on' as recorded before entering guest mode.
	 * Same as enter_from_user_mode().
	 *
	 * guest_exit_irqoff() restores host context and reinstates RCU if
	 * enabled and required.
	 *
	 * This needs to be done before the below as native_read_msr()
	 * contains a tracepoint and x86_spec_ctrl_restore_host() calls
	 * into world and some more.
	 */
	lockdep_hardirqs_off(CALLER_ADDR0);
	guest_exit_irqoff();

	instrumentation_begin();
	trace_hardirqs_off_finish();
	instrumentation_end();
}