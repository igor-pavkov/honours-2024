static void handle_invalid_guest_state(struct kvm_vcpu *vcpu,
				struct kvm_run *kvm_run)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	enum emulation_result err = EMULATE_DONE;

	preempt_enable();
	local_irq_enable();

	while (!guest_state_valid(vcpu)) {
		err = emulate_instruction(vcpu, kvm_run, 0, 0, 0);

		if (err == EMULATE_DO_MMIO)
			break;

		if (err != EMULATE_DONE) {
			kvm_report_emulation_failure(vcpu, "emulation failure");
			return;
		}

		if (signal_pending(current))
			break;
		if (need_resched())
			schedule();
	}

	local_irq_disable();
	preempt_disable();

	vmx->invalid_state_emulation_result = err;
}