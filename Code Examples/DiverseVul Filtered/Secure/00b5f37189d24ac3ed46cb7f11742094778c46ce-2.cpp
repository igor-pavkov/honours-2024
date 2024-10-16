void kvm_recalculate_apic_map(struct kvm *kvm)
{
	struct kvm_apic_map *new, *old = NULL;
	struct kvm_vcpu *vcpu;
	unsigned long i;
	u32 max_id = 255; /* enough space for any xAPIC ID */

	/* Read kvm->arch.apic_map_dirty before kvm->arch.apic_map.  */
	if (atomic_read_acquire(&kvm->arch.apic_map_dirty) == CLEAN)
		return;

	WARN_ONCE(!irqchip_in_kernel(kvm),
		  "Dirty APIC map without an in-kernel local APIC");

	mutex_lock(&kvm->arch.apic_map_lock);
	/*
	 * Read kvm->arch.apic_map_dirty before kvm->arch.apic_map
	 * (if clean) or the APIC registers (if dirty).
	 */
	if (atomic_cmpxchg_acquire(&kvm->arch.apic_map_dirty,
				   DIRTY, UPDATE_IN_PROGRESS) == CLEAN) {
		/* Someone else has updated the map. */
		mutex_unlock(&kvm->arch.apic_map_lock);
		return;
	}

	kvm_for_each_vcpu(i, vcpu, kvm)
		if (kvm_apic_present(vcpu))
			max_id = max(max_id, kvm_x2apic_id(vcpu->arch.apic));

	new = kvzalloc(sizeof(struct kvm_apic_map) +
	                   sizeof(struct kvm_lapic *) * ((u64)max_id + 1),
			   GFP_KERNEL_ACCOUNT);

	if (!new)
		goto out;

	new->max_apic_id = max_id;

	kvm_for_each_vcpu(i, vcpu, kvm) {
		struct kvm_lapic *apic = vcpu->arch.apic;
		struct kvm_lapic **cluster;
		u16 mask;
		u32 ldr;
		u8 xapic_id;
		u32 x2apic_id;

		if (!kvm_apic_present(vcpu))
			continue;

		xapic_id = kvm_xapic_id(apic);
		x2apic_id = kvm_x2apic_id(apic);

		/* Hotplug hack: see kvm_apic_match_physical_addr(), ... */
		if ((apic_x2apic_mode(apic) || x2apic_id > 0xff) &&
				x2apic_id <= new->max_apic_id)
			new->phys_map[x2apic_id] = apic;
		/*
		 * ... xAPIC ID of VCPUs with APIC ID > 0xff will wrap-around,
		 * prevent them from masking VCPUs with APIC ID <= 0xff.
		 */
		if (!apic_x2apic_mode(apic) && !new->phys_map[xapic_id])
			new->phys_map[xapic_id] = apic;

		if (!kvm_apic_sw_enabled(apic))
			continue;

		ldr = kvm_lapic_get_reg(apic, APIC_LDR);

		if (apic_x2apic_mode(apic)) {
			new->mode |= KVM_APIC_MODE_X2APIC;
		} else if (ldr) {
			ldr = GET_APIC_LOGICAL_ID(ldr);
			if (kvm_lapic_get_reg(apic, APIC_DFR) == APIC_DFR_FLAT)
				new->mode |= KVM_APIC_MODE_XAPIC_FLAT;
			else
				new->mode |= KVM_APIC_MODE_XAPIC_CLUSTER;
		}

		if (!kvm_apic_map_get_logical_dest(new, ldr, &cluster, &mask))
			continue;

		if (mask)
			cluster[ffs(mask) - 1] = apic;
	}
out:
	old = rcu_dereference_protected(kvm->arch.apic_map,
			lockdep_is_held(&kvm->arch.apic_map_lock));
	rcu_assign_pointer(kvm->arch.apic_map, new);
	/*
	 * Write kvm->arch.apic_map before clearing apic->apic_map_dirty.
	 * If another update has come in, leave it DIRTY.
	 */
	atomic_cmpxchg_release(&kvm->arch.apic_map_dirty,
			       UPDATE_IN_PROGRESS, CLEAN);
	mutex_unlock(&kvm->arch.apic_map_lock);

	if (old)
		call_rcu(&old->rcu, kvm_apic_map_free);

	kvm_make_scan_ioapic_request(kvm);
}