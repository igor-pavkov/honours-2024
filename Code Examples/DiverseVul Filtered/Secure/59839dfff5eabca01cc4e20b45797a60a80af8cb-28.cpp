static int kvm_vm_ioctl_reinject(struct kvm *kvm,
				 struct kvm_reinject_control *control)
{
	if (!kvm->arch.vpit)
		return -ENXIO;
	kvm->arch.vpit->pit_state.pit_timer.reinject = control->pit_reinject;
	return 0;
}