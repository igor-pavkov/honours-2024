void kvm_arch_destroy_vm(struct kvm *kvm)
{
	kvm_iommu_unmap_guest(kvm);
	kvm_free_pit(kvm);
	kfree(kvm->arch.vpic);
	kfree(kvm->arch.vioapic);
	kvm_free_vcpus(kvm);
	kvm_free_physmem(kvm);
	if (kvm->arch.apic_access_page)
		put_page(kvm->arch.apic_access_page);
	if (kvm->arch.ept_identity_pagetable)
		put_page(kvm->arch.ept_identity_pagetable);
	kfree(kvm);
}