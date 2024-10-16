static int kvm_vm_ioctl_get_pit(struct kvm *kvm, struct kvm_pit_state *ps)
{
	int r = 0;

	memcpy(ps, &kvm->arch.vpit->pit_state, sizeof(struct kvm_pit_state));
	return r;
}