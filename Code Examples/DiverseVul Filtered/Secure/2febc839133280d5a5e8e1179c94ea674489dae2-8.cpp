static void pit_do_work(struct kthread_work *work)
{
	struct kvm_pit *pit = container_of(work, struct kvm_pit, expired);
	struct kvm *kvm = pit->kvm;
	struct kvm_vcpu *vcpu;
	int i;
	struct kvm_kpit_state *ps = &pit->pit_state;
	int inject = 0;

	/* Try to inject pending interrupts when
	 * last one has been acked.
	 */
	spin_lock(&ps->inject_lock);
	if (ps->irq_ack) {
		ps->irq_ack = 0;
		inject = 1;
	}
	spin_unlock(&ps->inject_lock);
	if (inject) {
		kvm_set_irq(kvm, kvm->arch.vpit->irq_source_id, 0, 1, false);
		kvm_set_irq(kvm, kvm->arch.vpit->irq_source_id, 0, 0, false);

		/*
		 * Provides NMI watchdog support via Virtual Wire mode.
		 * The route is: PIT -> PIC -> LVT0 in NMI mode.
		 *
		 * Note: Our Virtual Wire implementation is simplified, only
		 * propagating PIT interrupts to all VCPUs when they have set
		 * LVT0 to NMI delivery. Other PIC interrupts are just sent to
		 * VCPU0, and only if its LVT0 is in EXTINT mode.
		 */
		if (kvm->arch.vapics_in_nmi_mode > 0)
			kvm_for_each_vcpu(i, vcpu, kvm)
				kvm_apic_nmi_wd_deliver(vcpu);
	}
}