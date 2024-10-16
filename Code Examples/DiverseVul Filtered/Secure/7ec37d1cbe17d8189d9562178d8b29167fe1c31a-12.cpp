void kvm_hv_set_cpuid(struct kvm_vcpu *vcpu)
{
	struct kvm_cpuid_entry2 *entry;
	struct kvm_vcpu_hv *hv_vcpu;

	entry = kvm_find_cpuid_entry(vcpu, HYPERV_CPUID_INTERFACE, 0);
	if (entry && entry->eax == HYPERV_CPUID_SIGNATURE_EAX) {
		vcpu->arch.hyperv_enabled = true;
	} else {
		vcpu->arch.hyperv_enabled = false;
		return;
	}

	if (!to_hv_vcpu(vcpu) && kvm_hv_vcpu_init(vcpu))
		return;

	hv_vcpu = to_hv_vcpu(vcpu);

	entry = kvm_find_cpuid_entry(vcpu, HYPERV_CPUID_FEATURES, 0);
	if (entry) {
		hv_vcpu->cpuid_cache.features_eax = entry->eax;
		hv_vcpu->cpuid_cache.features_ebx = entry->ebx;
		hv_vcpu->cpuid_cache.features_edx = entry->edx;
	} else {
		hv_vcpu->cpuid_cache.features_eax = 0;
		hv_vcpu->cpuid_cache.features_ebx = 0;
		hv_vcpu->cpuid_cache.features_edx = 0;
	}

	entry = kvm_find_cpuid_entry(vcpu, HYPERV_CPUID_ENLIGHTMENT_INFO, 0);
	if (entry) {
		hv_vcpu->cpuid_cache.enlightenments_eax = entry->eax;
		hv_vcpu->cpuid_cache.enlightenments_ebx = entry->ebx;
	} else {
		hv_vcpu->cpuid_cache.enlightenments_eax = 0;
		hv_vcpu->cpuid_cache.enlightenments_ebx = 0;
	}

	entry = kvm_find_cpuid_entry(vcpu, HYPERV_CPUID_SYNDBG_PLATFORM_CAPABILITIES, 0);
	if (entry)
		hv_vcpu->cpuid_cache.syndbg_cap_eax = entry->eax;
	else
		hv_vcpu->cpuid_cache.syndbg_cap_eax = 0;
}