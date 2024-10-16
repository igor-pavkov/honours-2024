static int vcpu_enter_guest(struct kvm_vcpu *vcpu)
{
	int r;
	bool req_int_win =
		dm_request_for_irq_injection(vcpu) &&
		kvm_cpu_accept_dm_intr(vcpu);

	bool req_immediate_exit = false;

	if (kvm_request_pending(vcpu)) {
		if (kvm_check_request(KVM_REQ_GET_VMCS12_PAGES, vcpu))
			kvm_x86_ops->get_vmcs12_pages(vcpu);
		if (kvm_check_request(KVM_REQ_MMU_RELOAD, vcpu))
			kvm_mmu_unload(vcpu);
		if (kvm_check_request(KVM_REQ_MIGRATE_TIMER, vcpu))
			__kvm_migrate_timers(vcpu);
		if (kvm_check_request(KVM_REQ_MASTERCLOCK_UPDATE, vcpu))
			kvm_gen_update_masterclock(vcpu->kvm);
		if (kvm_check_request(KVM_REQ_GLOBAL_CLOCK_UPDATE, vcpu))
			kvm_gen_kvmclock_update(vcpu);
		if (kvm_check_request(KVM_REQ_CLOCK_UPDATE, vcpu)) {
			r = kvm_guest_time_update(vcpu);
			if (unlikely(r))
				goto out;
		}
		if (kvm_check_request(KVM_REQ_MMU_SYNC, vcpu))
			kvm_mmu_sync_roots(vcpu);
		if (kvm_check_request(KVM_REQ_LOAD_CR3, vcpu))
			kvm_mmu_load_cr3(vcpu);
		if (kvm_check_request(KVM_REQ_TLB_FLUSH, vcpu))
			kvm_vcpu_flush_tlb(vcpu, true);
		if (kvm_check_request(KVM_REQ_REPORT_TPR_ACCESS, vcpu)) {
			vcpu->run->exit_reason = KVM_EXIT_TPR_ACCESS;
			r = 0;
			goto out;
		}
		if (kvm_check_request(KVM_REQ_TRIPLE_FAULT, vcpu)) {
			vcpu->run->exit_reason = KVM_EXIT_SHUTDOWN;
			vcpu->mmio_needed = 0;
			r = 0;
			goto out;
		}
		if (kvm_check_request(KVM_REQ_APF_HALT, vcpu)) {
			/* Page is swapped out. Do synthetic halt */
			vcpu->arch.apf.halted = true;
			r = 1;
			goto out;
		}
		if (kvm_check_request(KVM_REQ_STEAL_UPDATE, vcpu))
			record_steal_time(vcpu);
		if (kvm_check_request(KVM_REQ_SMI, vcpu))
			process_smi(vcpu);
		if (kvm_check_request(KVM_REQ_NMI, vcpu))
			process_nmi(vcpu);
		if (kvm_check_request(KVM_REQ_PMU, vcpu))
			kvm_pmu_handle_event(vcpu);
		if (kvm_check_request(KVM_REQ_PMI, vcpu))
			kvm_pmu_deliver_pmi(vcpu);
		if (kvm_check_request(KVM_REQ_IOAPIC_EOI_EXIT, vcpu)) {
			BUG_ON(vcpu->arch.pending_ioapic_eoi > 255);
			if (test_bit(vcpu->arch.pending_ioapic_eoi,
				     vcpu->arch.ioapic_handled_vectors)) {
				vcpu->run->exit_reason = KVM_EXIT_IOAPIC_EOI;
				vcpu->run->eoi.vector =
						vcpu->arch.pending_ioapic_eoi;
				r = 0;
				goto out;
			}
		}
		if (kvm_check_request(KVM_REQ_SCAN_IOAPIC, vcpu))
			vcpu_scan_ioapic(vcpu);
		if (kvm_check_request(KVM_REQ_LOAD_EOI_EXITMAP, vcpu))
			vcpu_load_eoi_exitmap(vcpu);
		if (kvm_check_request(KVM_REQ_APIC_PAGE_RELOAD, vcpu))
			kvm_vcpu_reload_apic_access_page(vcpu);
		if (kvm_check_request(KVM_REQ_HV_CRASH, vcpu)) {
			vcpu->run->exit_reason = KVM_EXIT_SYSTEM_EVENT;
			vcpu->run->system_event.type = KVM_SYSTEM_EVENT_CRASH;
			r = 0;
			goto out;
		}
		if (kvm_check_request(KVM_REQ_HV_RESET, vcpu)) {
			vcpu->run->exit_reason = KVM_EXIT_SYSTEM_EVENT;
			vcpu->run->system_event.type = KVM_SYSTEM_EVENT_RESET;
			r = 0;
			goto out;
		}
		if (kvm_check_request(KVM_REQ_HV_EXIT, vcpu)) {
			vcpu->run->exit_reason = KVM_EXIT_HYPERV;
			vcpu->run->hyperv = vcpu->arch.hyperv.exit;
			r = 0;
			goto out;
		}

		/*
		 * KVM_REQ_HV_STIMER has to be processed after
		 * KVM_REQ_CLOCK_UPDATE, because Hyper-V SynIC timers
		 * depend on the guest clock being up-to-date
		 */
		if (kvm_check_request(KVM_REQ_HV_STIMER, vcpu))
			kvm_hv_process_stimers(vcpu);
	}

	if (kvm_check_request(KVM_REQ_EVENT, vcpu) || req_int_win) {
		++vcpu->stat.req_event;
		kvm_apic_accept_events(vcpu);
		if (vcpu->arch.mp_state == KVM_MP_STATE_INIT_RECEIVED) {
			r = 1;
			goto out;
		}

		if (inject_pending_event(vcpu, req_int_win) != 0)
			req_immediate_exit = true;
		else {
			/* Enable SMI/NMI/IRQ window open exits if needed.
			 *
			 * SMIs have three cases:
			 * 1) They can be nested, and then there is nothing to
			 *    do here because RSM will cause a vmexit anyway.
			 * 2) There is an ISA-specific reason why SMI cannot be
			 *    injected, and the moment when this changes can be
			 *    intercepted.
			 * 3) Or the SMI can be pending because
			 *    inject_pending_event has completed the injection
			 *    of an IRQ or NMI from the previous vmexit, and
			 *    then we request an immediate exit to inject the
			 *    SMI.
			 */
			if (vcpu->arch.smi_pending && !is_smm(vcpu))
				if (!kvm_x86_ops->enable_smi_window(vcpu))
					req_immediate_exit = true;
			if (vcpu->arch.nmi_pending)
				kvm_x86_ops->enable_nmi_window(vcpu);
			if (kvm_cpu_has_injectable_intr(vcpu) || req_int_win)
				kvm_x86_ops->enable_irq_window(vcpu);
			WARN_ON(vcpu->arch.exception.pending);
		}

		if (kvm_lapic_enabled(vcpu)) {
			update_cr8_intercept(vcpu);
			kvm_lapic_sync_to_vapic(vcpu);
		}
	}

	r = kvm_mmu_reload(vcpu);
	if (unlikely(r)) {
		goto cancel_injection;
	}

	preempt_disable();

	kvm_x86_ops->prepare_guest_switch(vcpu);

	/*
	 * Disable IRQs before setting IN_GUEST_MODE.  Posted interrupt
	 * IPI are then delayed after guest entry, which ensures that they
	 * result in virtual interrupt delivery.
	 */
	local_irq_disable();
	vcpu->mode = IN_GUEST_MODE;

	srcu_read_unlock(&vcpu->kvm->srcu, vcpu->srcu_idx);

	/*
	 * 1) We should set ->mode before checking ->requests.  Please see
	 * the comment in kvm_vcpu_exiting_guest_mode().
	 *
	 * 2) For APICv, we should set ->mode before checking PIR.ON.  This
	 * pairs with the memory barrier implicit in pi_test_and_set_on
	 * (see vmx_deliver_posted_interrupt).
	 *
	 * 3) This also orders the write to mode from any reads to the page
	 * tables done while the VCPU is running.  Please see the comment
	 * in kvm_flush_remote_tlbs.
	 */
	smp_mb__after_srcu_read_unlock();

	/*
	 * This handles the case where a posted interrupt was
	 * notified with kvm_vcpu_kick.
	 */
	if (kvm_lapic_enabled(vcpu) && vcpu->arch.apicv_active)
		kvm_x86_ops->sync_pir_to_irr(vcpu);

	if (vcpu->mode == EXITING_GUEST_MODE || kvm_request_pending(vcpu)
	    || need_resched() || signal_pending(current)) {
		vcpu->mode = OUTSIDE_GUEST_MODE;
		smp_wmb();
		local_irq_enable();
		preempt_enable();
		vcpu->srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);
		r = 1;
		goto cancel_injection;
	}

	kvm_load_guest_xcr0(vcpu);

	if (req_immediate_exit) {
		kvm_make_request(KVM_REQ_EVENT, vcpu);
		kvm_x86_ops->request_immediate_exit(vcpu);
	}

	trace_kvm_entry(vcpu->vcpu_id);
	if (lapic_timer_advance_ns)
		wait_lapic_expire(vcpu);
	guest_enter_irqoff();

	if (unlikely(vcpu->arch.switch_db_regs)) {
		set_debugreg(0, 7);
		set_debugreg(vcpu->arch.eff_db[0], 0);
		set_debugreg(vcpu->arch.eff_db[1], 1);
		set_debugreg(vcpu->arch.eff_db[2], 2);
		set_debugreg(vcpu->arch.eff_db[3], 3);
		set_debugreg(vcpu->arch.dr6, 6);
		vcpu->arch.switch_db_regs &= ~KVM_DEBUGREG_RELOAD;
	}

	kvm_x86_ops->run(vcpu);

	/*
	 * Do this here before restoring debug registers on the host.  And
	 * since we do this before handling the vmexit, a DR access vmexit
	 * can (a) read the correct value of the debug registers, (b) set
	 * KVM_DEBUGREG_WONT_EXIT again.
	 */
	if (unlikely(vcpu->arch.switch_db_regs & KVM_DEBUGREG_WONT_EXIT)) {
		WARN_ON(vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP);
		kvm_x86_ops->sync_dirty_debug_regs(vcpu);
		kvm_update_dr0123(vcpu);
		kvm_update_dr6(vcpu);
		kvm_update_dr7(vcpu);
		vcpu->arch.switch_db_regs &= ~KVM_DEBUGREG_RELOAD;
	}

	/*
	 * If the guest has used debug registers, at least dr7
	 * will be disabled while returning to the host.
	 * If we don't have active breakpoints in the host, we don't
	 * care about the messed up debug address registers. But if
	 * we have some of them active, restore the old state.
	 */
	if (hw_breakpoint_active())
		hw_breakpoint_restore();

	vcpu->arch.last_guest_tsc = kvm_read_l1_tsc(vcpu, rdtsc());

	vcpu->mode = OUTSIDE_GUEST_MODE;
	smp_wmb();

	kvm_put_guest_xcr0(vcpu);

	kvm_before_interrupt(vcpu);
	kvm_x86_ops->handle_external_intr(vcpu);
	kvm_after_interrupt(vcpu);

	++vcpu->stat.exits;

	guest_exit_irqoff();

	local_irq_enable();
	preempt_enable();

	vcpu->srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);

	/*
	 * Profile KVM exit RIPs:
	 */
	if (unlikely(prof_on == KVM_PROFILING)) {
		unsigned long rip = kvm_rip_read(vcpu);
		profile_hit(KVM_PROFILING, (void *)rip);
	}

	if (unlikely(vcpu->arch.tsc_always_catchup))
		kvm_make_request(KVM_REQ_CLOCK_UPDATE, vcpu);

	if (vcpu->arch.apic_attention)
		kvm_lapic_sync_from_vapic(vcpu);

	vcpu->arch.gpa_available = false;
	r = kvm_x86_ops->handle_exit(vcpu);
	return r;

cancel_injection:
	kvm_x86_ops->cancel_injection(vcpu);
	if (unlikely(vcpu->arch.apic_attention))
		kvm_lapic_sync_from_vapic(vcpu);
out:
	return r;
}