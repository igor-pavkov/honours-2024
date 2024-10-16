static int vcpu_enter_guest(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	int r;

	if (vcpu->requests)
		if (test_and_clear_bit(KVM_REQ_MMU_RELOAD, &vcpu->requests))
			kvm_mmu_unload(vcpu);

	r = kvm_mmu_reload(vcpu);
	if (unlikely(r))
		goto out;

	if (vcpu->requests) {
		if (test_and_clear_bit(KVM_REQ_MIGRATE_TIMER, &vcpu->requests))
			__kvm_migrate_timers(vcpu);
		if (test_and_clear_bit(KVM_REQ_KVMCLOCK_UPDATE, &vcpu->requests))
			kvm_write_guest_time(vcpu);
		if (test_and_clear_bit(KVM_REQ_MMU_SYNC, &vcpu->requests))
			kvm_mmu_sync_roots(vcpu);
		if (test_and_clear_bit(KVM_REQ_TLB_FLUSH, &vcpu->requests))
			kvm_x86_ops->tlb_flush(vcpu);
		if (test_and_clear_bit(KVM_REQ_REPORT_TPR_ACCESS,
				       &vcpu->requests)) {
			kvm_run->exit_reason = KVM_EXIT_TPR_ACCESS;
			r = 0;
			goto out;
		}
		if (test_and_clear_bit(KVM_REQ_TRIPLE_FAULT, &vcpu->requests)) {
			kvm_run->exit_reason = KVM_EXIT_SHUTDOWN;
			r = 0;
			goto out;
		}
	}

	preempt_disable();

	kvm_x86_ops->prepare_guest_switch(vcpu);
	kvm_load_guest_fpu(vcpu);

	local_irq_disable();

	if (vcpu->requests || need_resched() || signal_pending(current)) {
		local_irq_enable();
		preempt_enable();
		r = 1;
		goto out;
	}

	vcpu->guest_mode = 1;
	/*
	 * Make sure that guest_mode assignment won't happen after
	 * testing the pending IRQ vector bitmap.
	 */
	smp_wmb();

	if (vcpu->arch.exception.pending)
		__queue_exception(vcpu);
	else if (irqchip_in_kernel(vcpu->kvm))
		kvm_x86_ops->inject_pending_irq(vcpu);
	else
		kvm_x86_ops->inject_pending_vectors(vcpu, kvm_run);

	kvm_lapic_sync_to_vapic(vcpu);

	up_read(&vcpu->kvm->slots_lock);

	kvm_guest_enter();

	get_debugreg(vcpu->arch.host_dr6, 6);
	get_debugreg(vcpu->arch.host_dr7, 7);
	if (unlikely(vcpu->arch.switch_db_regs)) {
		get_debugreg(vcpu->arch.host_db[0], 0);
		get_debugreg(vcpu->arch.host_db[1], 1);
		get_debugreg(vcpu->arch.host_db[2], 2);
		get_debugreg(vcpu->arch.host_db[3], 3);

		set_debugreg(0, 7);
		set_debugreg(vcpu->arch.eff_db[0], 0);
		set_debugreg(vcpu->arch.eff_db[1], 1);
		set_debugreg(vcpu->arch.eff_db[2], 2);
		set_debugreg(vcpu->arch.eff_db[3], 3);
	}

	KVMTRACE_0D(VMENTRY, vcpu, entryexit);
	kvm_x86_ops->run(vcpu, kvm_run);

	if (unlikely(vcpu->arch.switch_db_regs)) {
		set_debugreg(0, 7);
		set_debugreg(vcpu->arch.host_db[0], 0);
		set_debugreg(vcpu->arch.host_db[1], 1);
		set_debugreg(vcpu->arch.host_db[2], 2);
		set_debugreg(vcpu->arch.host_db[3], 3);
	}
	set_debugreg(vcpu->arch.host_dr6, 6);
	set_debugreg(vcpu->arch.host_dr7, 7);

	vcpu->guest_mode = 0;
	local_irq_enable();

	++vcpu->stat.exits;

	/*
	 * We must have an instruction between local_irq_enable() and
	 * kvm_guest_exit(), so the timer interrupt isn't delayed by
	 * the interrupt shadow.  The stat.exits increment will do nicely.
	 * But we need to prevent reordering, hence this barrier():
	 */
	barrier();

	kvm_guest_exit();

	preempt_enable();

	down_read(&vcpu->kvm->slots_lock);

	/*
	 * Profile KVM exit RIPs:
	 */
	if (unlikely(prof_on == KVM_PROFILING)) {
		unsigned long rip = kvm_rip_read(vcpu);
		profile_hit(KVM_PROFILING, (void *)rip);
	}

	if (vcpu->arch.exception.pending && kvm_x86_ops->exception_injected(vcpu))
		vcpu->arch.exception.pending = false;

	kvm_lapic_sync_from_vapic(vcpu);

	r = kvm_x86_ops->handle_exit(kvm_run, vcpu);
out:
	return r;
}