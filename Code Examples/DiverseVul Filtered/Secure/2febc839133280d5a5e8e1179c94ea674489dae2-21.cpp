static void kvm_pit_ack_irq(struct kvm_irq_ack_notifier *kian)
{
	struct kvm_kpit_state *ps = container_of(kian, struct kvm_kpit_state,
						 irq_ack_notifier);
	int value;

	spin_lock(&ps->inject_lock);
	value = atomic_dec_return(&ps->pending);
	if (value < 0)
		/* spurious acks can be generated if, for example, the
		 * PIC is being reset.  Handle it gracefully here
		 */
		atomic_inc(&ps->pending);
	else if (value > 0)
		/* in this case, we had multiple outstanding pit interrupts
		 * that we needed to inject.  Reinject
		 */
		queue_kthread_work(&ps->pit->worker, &ps->pit->expired);
	ps->irq_ack = 1;
	spin_unlock(&ps->inject_lock);
}