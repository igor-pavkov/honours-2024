void kvm_lapic_set_base(struct kvm_vcpu *vcpu, u64 value)
{
	u64 old_value = vcpu->arch.apic_base;
	struct kvm_lapic *apic = vcpu->arch.apic;

	vcpu->arch.apic_base = value;

	if ((old_value ^ value) & MSR_IA32_APICBASE_ENABLE)
		kvm_update_cpuid_runtime(vcpu);

	if (!apic)
		return;

	/* update jump label if enable bit changes */
	if ((old_value ^ value) & MSR_IA32_APICBASE_ENABLE) {
		if (value & MSR_IA32_APICBASE_ENABLE) {
			kvm_apic_set_xapic_id(apic, vcpu->vcpu_id);
			static_branch_slow_dec_deferred(&apic_hw_disabled);
			/* Check if there are APF page ready requests pending */
			kvm_make_request(KVM_REQ_APF_READY, vcpu);
		} else {
			static_branch_inc(&apic_hw_disabled.key);
			atomic_set_release(&apic->vcpu->kvm->arch.apic_map_dirty, DIRTY);
		}
	}

	if (((old_value ^ value) & X2APIC_ENABLE) && (value & X2APIC_ENABLE))
		kvm_apic_set_x2apic_id(apic, vcpu->vcpu_id);

	if ((old_value ^ value) & (MSR_IA32_APICBASE_ENABLE | X2APIC_ENABLE))
		static_call_cond(kvm_x86_set_virtual_apic_mode)(vcpu);

	apic->base_address = apic->vcpu->arch.apic_base &
			     MSR_IA32_APICBASE_BASE;

	if ((value & MSR_IA32_APICBASE_ENABLE) &&
	     apic->base_address != APIC_DEFAULT_PHYS_BASE)
		pr_warn_once("APIC base relocation is unsupported by KVM");
}