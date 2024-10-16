static void svm_vm_destroy(struct kvm *kvm)
{
	avic_vm_destroy(kvm);
	sev_vm_destroy(kvm);
}