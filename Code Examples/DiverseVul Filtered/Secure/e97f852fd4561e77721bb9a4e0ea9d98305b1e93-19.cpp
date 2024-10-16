static int kvm_vcpu_ioctl_enable_cap(struct kvm_vcpu *vcpu,
				     struct kvm_enable_cap *cap)
{
	int r;
	uint16_t vmcs_version;
	void __user *user_ptr;

	if (cap->flags)
		return -EINVAL;

	switch (cap->cap) {
	case KVM_CAP_HYPERV_SYNIC2:
		if (cap->args[0])
			return -EINVAL;
	case KVM_CAP_HYPERV_SYNIC:
		if (!irqchip_in_kernel(vcpu->kvm))
			return -EINVAL;
		return kvm_hv_activate_synic(vcpu, cap->cap ==
					     KVM_CAP_HYPERV_SYNIC2);
	case KVM_CAP_HYPERV_ENLIGHTENED_VMCS:
		r = kvm_x86_ops->nested_enable_evmcs(vcpu, &vmcs_version);
		if (!r) {
			user_ptr = (void __user *)(uintptr_t)cap->args[0];
			if (copy_to_user(user_ptr, &vmcs_version,
					 sizeof(vmcs_version)))
				r = -EFAULT;
		}
		return r;

	default:
		return -EINVAL;
	}
}