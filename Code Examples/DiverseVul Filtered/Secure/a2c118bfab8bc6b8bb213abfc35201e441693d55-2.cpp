void kvm_ioapic_make_eoibitmap_request(struct kvm *kvm)
{
	struct kvm_ioapic *ioapic = kvm->arch.vioapic;

	if (!kvm_apic_vid_enabled(kvm) || !ioapic)
		return;
	kvm_make_update_eoibitmap_request(kvm);
}