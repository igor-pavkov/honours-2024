static int megasas_mgmt_ioctl_fw(struct file *file, unsigned long arg)
{
	struct megasas_iocpacket __user *user_ioc =
	    (struct megasas_iocpacket __user *)arg;
	struct megasas_iocpacket *ioc;
	struct megasas_instance *instance;
	int error;

	ioc = memdup_user(user_ioc, sizeof(*ioc));
	if (IS_ERR(ioc))
		return PTR_ERR(ioc);

	instance = megasas_lookup_instance(ioc->host_no);
	if (!instance) {
		error = -ENODEV;
		goto out_kfree_ioc;
	}

	/* Block ioctls in VF mode */
	if (instance->requestorId && !allow_vf_ioctls) {
		error = -ENODEV;
		goto out_kfree_ioc;
	}

	if (atomic_read(&instance->adprecovery) == MEGASAS_HW_CRITICAL_ERROR) {
		dev_err(&instance->pdev->dev, "Controller in crit error\n");
		error = -ENODEV;
		goto out_kfree_ioc;
	}

	if (instance->unload == 1) {
		error = -ENODEV;
		goto out_kfree_ioc;
	}

	if (down_interruptible(&instance->ioctl_sem)) {
		error = -ERESTARTSYS;
		goto out_kfree_ioc;
	}

	if  (megasas_wait_for_adapter_operational(instance)) {
		error = -ENODEV;
		goto out_up;
	}

	error = megasas_mgmt_fw_ioctl(instance, user_ioc, ioc);
out_up:
	up(&instance->ioctl_sem);

out_kfree_ioc:
	kfree(ioc);
	return error;
}