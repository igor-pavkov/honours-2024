void kvm_arch_sync_events(struct kvm *kvm)
{
	kvm_free_all_assigned_devices(kvm);
}