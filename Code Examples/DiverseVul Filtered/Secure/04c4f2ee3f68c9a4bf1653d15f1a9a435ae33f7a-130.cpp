static int handle_dr(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification;
	int dr, dr7, reg;
	int err = 1;

	exit_qualification = vmx_get_exit_qual(vcpu);
	dr = exit_qualification & DEBUG_REG_ACCESS_NUM;

	/* First, if DR does not exist, trigger UD */
	if (!kvm_require_dr(vcpu, dr))
		return 1;

	if (kvm_x86_ops.get_cpl(vcpu) > 0)
		goto out;

	dr7 = vmcs_readl(GUEST_DR7);
	if (dr7 & DR7_GD) {
		/*
		 * As the vm-exit takes precedence over the debug trap, we
		 * need to emulate the latter, either for the host or the
		 * guest debugging itself.
		 */
		if (vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP) {
			vcpu->run->debug.arch.dr6 = DR6_BD | DR6_ACTIVE_LOW;
			vcpu->run->debug.arch.dr7 = dr7;
			vcpu->run->debug.arch.pc = kvm_get_linear_rip(vcpu);
			vcpu->run->debug.arch.exception = DB_VECTOR;
			vcpu->run->exit_reason = KVM_EXIT_DEBUG;
			return 0;
		} else {
			kvm_queue_exception_p(vcpu, DB_VECTOR, DR6_BD);
			return 1;
		}
	}

	if (vcpu->guest_debug == 0) {
		exec_controls_clearbit(to_vmx(vcpu), CPU_BASED_MOV_DR_EXITING);

		/*
		 * No more DR vmexits; force a reload of the debug registers
		 * and reenter on this instruction.  The next vmexit will
		 * retrieve the full state of the debug registers.
		 */
		vcpu->arch.switch_db_regs |= KVM_DEBUGREG_WONT_EXIT;
		return 1;
	}

	reg = DEBUG_REG_ACCESS_REG(exit_qualification);
	if (exit_qualification & TYPE_MOV_FROM_DR) {
		unsigned long val;

		kvm_get_dr(vcpu, dr, &val);
		kvm_register_write(vcpu, reg, val);
		err = 0;
	} else {
		err = kvm_set_dr(vcpu, dr, kvm_register_readl(vcpu, reg));
	}

out:
	return kvm_complete_insn_gp(vcpu, err);
}