static struct kvm *kvm_create_vm(void)
{
	int r, i;
	struct kvm *kvm = kvm_arch_alloc_vm();

	if (!kvm)
		return ERR_PTR(-ENOMEM);

	r = kvm_arch_init_vm(kvm);
	if (r)
		goto out_err_nodisable;

	r = hardware_enable_all();
	if (r)
		goto out_err_nodisable;

#ifdef CONFIG_HAVE_KVM_IRQCHIP
	INIT_HLIST_HEAD(&kvm->mask_notifier_list);
	INIT_HLIST_HEAD(&kvm->irq_ack_notifier_list);
#endif

	r = -ENOMEM;
	kvm->memslots = kzalloc(sizeof(struct kvm_memslots), GFP_KERNEL);
	if (!kvm->memslots)
		goto out_err_nosrcu;
	if (init_srcu_struct(&kvm->srcu))
		goto out_err_nosrcu;
	for (i = 0; i < KVM_NR_BUSES; i++) {
		kvm->buses[i] = kzalloc(sizeof(struct kvm_io_bus),
					GFP_KERNEL);
		if (!kvm->buses[i])
			goto out_err;
	}
	spin_lock_init(&kvm->mmu_lock);

	r = kvm_init_mmu_notifier(kvm);
	if (r)
		goto out_err;

	kvm->mm = current->mm;
	atomic_inc(&kvm->mm->mm_count);
	kvm_eventfd_init(kvm);
	mutex_init(&kvm->lock);
	mutex_init(&kvm->irq_lock);
	mutex_init(&kvm->slots_lock);
	atomic_set(&kvm->users_count, 1);
	raw_spin_lock(&kvm_lock);
	list_add(&kvm->vm_list, &vm_list);
	raw_spin_unlock(&kvm_lock);

	return kvm;

out_err:
	cleanup_srcu_struct(&kvm->srcu);
out_err_nosrcu:
	hardware_disable_all();
out_err_nodisable:
	for (i = 0; i < KVM_NR_BUSES; i++)
		kfree(kvm->buses[i]);
	kfree(kvm->memslots);
	kvm_arch_free_vm(kvm);
	return ERR_PTR(r);
}