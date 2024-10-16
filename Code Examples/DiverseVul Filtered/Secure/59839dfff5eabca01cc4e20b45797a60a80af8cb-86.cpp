int kvm_arch_vcpu_ioctl_set_guest_debug(struct kvm_vcpu *vcpu,
					struct kvm_guest_debug *dbg)
{
	int i, r;

	vcpu_load(vcpu);

	if ((dbg->control & (KVM_GUESTDBG_ENABLE | KVM_GUESTDBG_USE_HW_BP)) ==
	    (KVM_GUESTDBG_ENABLE | KVM_GUESTDBG_USE_HW_BP)) {
		for (i = 0; i < KVM_NR_DB_REGS; ++i)
			vcpu->arch.eff_db[i] = dbg->arch.debugreg[i];
		vcpu->arch.switch_db_regs =
			(dbg->arch.debugreg[7] & DR7_BP_EN_MASK);
	} else {
		for (i = 0; i < KVM_NR_DB_REGS; i++)
			vcpu->arch.eff_db[i] = vcpu->arch.db[i];
		vcpu->arch.switch_db_regs = (vcpu->arch.dr7 & DR7_BP_EN_MASK);
	}

	r = kvm_x86_ops->set_guest_debug(vcpu, dbg);

	if (dbg->control & KVM_GUESTDBG_INJECT_DB)
		kvm_queue_exception(vcpu, DB_VECTOR);
	else if (dbg->control & KVM_GUESTDBG_INJECT_BP)
		kvm_queue_exception(vcpu, BP_VECTOR);

	vcpu_put(vcpu);

	return r;
}