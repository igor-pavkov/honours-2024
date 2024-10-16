static int kvm_ioctl_create_device(struct kvm *kvm,
				   struct kvm_create_device *cd)
{
	struct kvm_device_ops *ops = NULL;
	struct kvm_device *dev;
	bool test = cd->flags & KVM_CREATE_DEVICE_TEST;
	int ret;

	switch (cd->type) {
#ifdef CONFIG_KVM_MPIC
	case KVM_DEV_TYPE_FSL_MPIC_20:
	case KVM_DEV_TYPE_FSL_MPIC_42:
		ops = &kvm_mpic_ops;
		break;
#endif
#ifdef CONFIG_KVM_XICS
	case KVM_DEV_TYPE_XICS:
		ops = &kvm_xics_ops;
		break;
#endif
#ifdef CONFIG_KVM_VFIO
	case KVM_DEV_TYPE_VFIO:
		ops = &kvm_vfio_ops;
		break;
#endif
	default:
		return -ENODEV;
	}

	if (test)
		return 0;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	dev->ops = ops;
	dev->kvm = kvm;

	ret = ops->create(dev, cd->type);
	if (ret < 0) {
		kfree(dev);
		return ret;
	}

	ret = anon_inode_getfd(ops->name, &kvm_device_fops, dev, O_RDWR | O_CLOEXEC);
	if (ret < 0) {
		ops->destroy(dev);
		return ret;
	}

	list_add(&dev->vm_node, &kvm->devices);
	kvm_get_kvm(kvm);
	cd->fd = ret;
	return 0;
}