static void pvclock_gtod_update_fn(struct work_struct *work)
{
	struct kvm *kvm;

	struct kvm_vcpu *vcpu;
	int i;

	raw_spin_lock(&kvm_lock);
	list_for_each_entry(kvm, &vm_list, vm_list)
		kvm_for_each_vcpu(i, vcpu, kvm)
			set_bit(KVM_REQ_MASTERCLOCK_UPDATE, &vcpu->requests);
	atomic_set(&kvm_guest_has_master_clock, 0);
	raw_spin_unlock(&kvm_lock);
}