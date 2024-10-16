void kvm_bitmap_or_dest_vcpus(struct kvm *kvm, struct kvm_lapic_irq *irq,
			      unsigned long *vcpu_bitmap)
{
	struct kvm_lapic **dest_vcpu = NULL;
	struct kvm_lapic *src = NULL;
	struct kvm_apic_map *map;
	struct kvm_vcpu *vcpu;
	unsigned long bitmap, i;
	int vcpu_idx;
	bool ret;

	rcu_read_lock();
	map = rcu_dereference(kvm->arch.apic_map);

	ret = kvm_apic_map_get_dest_lapic(kvm, &src, irq, map, &dest_vcpu,
					  &bitmap);
	if (ret) {
		for_each_set_bit(i, &bitmap, 16) {
			if (!dest_vcpu[i])
				continue;
			vcpu_idx = dest_vcpu[i]->vcpu->vcpu_idx;
			__set_bit(vcpu_idx, vcpu_bitmap);
		}
	} else {
		kvm_for_each_vcpu(i, vcpu, kvm) {
			if (!kvm_apic_present(vcpu))
				continue;
			if (!kvm_apic_match_dest(vcpu, NULL,
						 irq->shorthand,
						 irq->dest_id,
						 irq->dest_mode))
				continue;
			__set_bit(i, vcpu_bitmap);
		}
	}
	rcu_read_unlock();
}