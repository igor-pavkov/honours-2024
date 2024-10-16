static long kvm_vcpu_compat_ioctl(struct file *filp,
				  unsigned int ioctl, unsigned long arg)
{
	struct kvm_vcpu *vcpu = filp->private_data;
	void __user *argp = compat_ptr(arg);
	int r;

	if (vcpu->kvm->mm != current->mm)
		return -EIO;

	switch (ioctl) {
	case KVM_SET_SIGNAL_MASK: {
		struct kvm_signal_mask __user *sigmask_arg = argp;
		struct kvm_signal_mask kvm_sigmask;
		compat_sigset_t csigset;
		sigset_t sigset;

		if (argp) {
			r = -EFAULT;
			if (copy_from_user(&kvm_sigmask, argp,
					   sizeof kvm_sigmask))
				goto out;
			r = -EINVAL;
			if (kvm_sigmask.len != sizeof csigset)
				goto out;
			r = -EFAULT;
			if (copy_from_user(&csigset, sigmask_arg->sigset,
					   sizeof csigset))
				goto out;
			sigset_from_compat(&sigset, &csigset);
			r = kvm_vcpu_ioctl_set_sigmask(vcpu, &sigset);
		} else
			r = kvm_vcpu_ioctl_set_sigmask(vcpu, NULL);
		break;
	}
	default:
		r = kvm_vcpu_ioctl(filp, ioctl, arg);
	}

out:
	return r;
}