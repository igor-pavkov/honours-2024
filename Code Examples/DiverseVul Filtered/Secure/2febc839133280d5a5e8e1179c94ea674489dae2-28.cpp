void kvm_free_pit(struct kvm *kvm)
{
	struct hrtimer *timer;

	if (kvm->arch.vpit) {
		kvm_io_bus_unregister_dev(kvm, KVM_PIO_BUS, &kvm->arch.vpit->dev);
		kvm_io_bus_unregister_dev(kvm, KVM_PIO_BUS,
					      &kvm->arch.vpit->speaker_dev);
		kvm_unregister_irq_mask_notifier(kvm, 0,
					       &kvm->arch.vpit->mask_notifier);
		kvm_unregister_irq_ack_notifier(kvm,
				&kvm->arch.vpit->pit_state.irq_ack_notifier);
		mutex_lock(&kvm->arch.vpit->pit_state.lock);
		timer = &kvm->arch.vpit->pit_state.timer;
		hrtimer_cancel(timer);
		flush_kthread_work(&kvm->arch.vpit->expired);
		kthread_stop(kvm->arch.vpit->worker_task);
		kvm_free_irq_source_id(kvm, kvm->arch.vpit->irq_source_id);
		mutex_unlock(&kvm->arch.vpit->pit_state.lock);
		kfree(kvm->arch.vpit);
	}
}