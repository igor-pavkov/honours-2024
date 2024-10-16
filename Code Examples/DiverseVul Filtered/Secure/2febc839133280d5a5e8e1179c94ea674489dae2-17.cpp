struct kvm_pit *kvm_create_pit(struct kvm *kvm, u32 flags)
{
	struct kvm_pit *pit;
	struct kvm_kpit_state *pit_state;
	struct pid *pid;
	pid_t pid_nr;
	int ret;

	pit = kzalloc(sizeof(struct kvm_pit), GFP_KERNEL);
	if (!pit)
		return NULL;

	pit->irq_source_id = kvm_request_irq_source_id(kvm);
	if (pit->irq_source_id < 0) {
		kfree(pit);
		return NULL;
	}

	mutex_init(&pit->pit_state.lock);
	mutex_lock(&pit->pit_state.lock);
	spin_lock_init(&pit->pit_state.inject_lock);

	pid = get_pid(task_tgid(current));
	pid_nr = pid_vnr(pid);
	put_pid(pid);

	init_kthread_worker(&pit->worker);
	pit->worker_task = kthread_run(kthread_worker_fn, &pit->worker,
				       "kvm-pit/%d", pid_nr);
	if (IS_ERR(pit->worker_task)) {
		mutex_unlock(&pit->pit_state.lock);
		kvm_free_irq_source_id(kvm, pit->irq_source_id);
		kfree(pit);
		return NULL;
	}
	init_kthread_work(&pit->expired, pit_do_work);

	kvm->arch.vpit = pit;
	pit->kvm = kvm;

	pit_state = &pit->pit_state;
	pit_state->pit = pit;
	hrtimer_init(&pit_state->timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS);
	pit_state->irq_ack_notifier.gsi = 0;
	pit_state->irq_ack_notifier.irq_acked = kvm_pit_ack_irq;
	kvm_register_irq_ack_notifier(kvm, &pit_state->irq_ack_notifier);
	pit_state->reinject = true;
	mutex_unlock(&pit->pit_state.lock);

	kvm_pit_reset(pit);

	pit->mask_notifier.func = pit_mask_notifer;
	kvm_register_irq_mask_notifier(kvm, 0, &pit->mask_notifier);

	kvm_iodevice_init(&pit->dev, &pit_dev_ops);
	ret = kvm_io_bus_register_dev(kvm, KVM_PIO_BUS, KVM_PIT_BASE_ADDRESS,
				      KVM_PIT_MEM_LENGTH, &pit->dev);
	if (ret < 0)
		goto fail;

	if (flags & KVM_PIT_SPEAKER_DUMMY) {
		kvm_iodevice_init(&pit->speaker_dev, &speaker_dev_ops);
		ret = kvm_io_bus_register_dev(kvm, KVM_PIO_BUS,
					      KVM_SPEAKER_BASE_ADDRESS, 4,
					      &pit->speaker_dev);
		if (ret < 0)
			goto fail_unregister;
	}

	return pit;

fail_unregister:
	kvm_io_bus_unregister_dev(kvm, KVM_PIO_BUS, &pit->dev);

fail:
	kvm_unregister_irq_mask_notifier(kvm, 0, &pit->mask_notifier);
	kvm_unregister_irq_ack_notifier(kvm, &pit_state->irq_ack_notifier);
	kvm_free_irq_source_id(kvm, pit->irq_source_id);
	kthread_stop(pit->worker_task);
	kfree(pit);
	return NULL;
}