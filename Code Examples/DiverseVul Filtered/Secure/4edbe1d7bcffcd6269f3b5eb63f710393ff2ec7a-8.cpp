static int copy_params(struct dm_ioctl __user *user, struct dm_ioctl *param_kernel,
		       int ioctl_flags, struct dm_ioctl **param, int *param_flags)
{
	struct dm_ioctl *dmi;
	int secure_data;
	const size_t minimum_data_size = offsetof(struct dm_ioctl, data);
	unsigned noio_flag;

	if (copy_from_user(param_kernel, user, minimum_data_size))
		return -EFAULT;

	if (param_kernel->data_size < minimum_data_size)
		return -EINVAL;

	secure_data = param_kernel->flags & DM_SECURE_DATA_FLAG;

	*param_flags = secure_data ? DM_WIPE_BUFFER : 0;

	if (ioctl_flags & IOCTL_FLAGS_NO_PARAMS) {
		dmi = param_kernel;
		dmi->data_size = minimum_data_size;
		goto data_copied;
	}

	/*
	 * Use __GFP_HIGH to avoid low memory issues when a device is
	 * suspended and the ioctl is needed to resume it.
	 * Use kmalloc() rather than vmalloc() when we can.
	 */
	dmi = NULL;
	noio_flag = memalloc_noio_save();
	dmi = kvmalloc(param_kernel->data_size, GFP_KERNEL | __GFP_HIGH);
	memalloc_noio_restore(noio_flag);

	if (!dmi) {
		if (secure_data && clear_user(user, param_kernel->data_size))
			return -EFAULT;
		return -ENOMEM;
	}

	*param_flags |= DM_PARAMS_MALLOC;

	/* Copy from param_kernel (which was already copied from user) */
	memcpy(dmi, param_kernel, minimum_data_size);

	if (copy_from_user(&dmi->data, (char __user *)user + minimum_data_size,
			   param_kernel->data_size - minimum_data_size))
		goto bad;
data_copied:
	/* Wipe the user buffer so we do not return it to userspace */
	if (secure_data && clear_user(user, param_kernel->data_size))
		goto bad;

	*param = dmi;
	return 0;

bad:
	free_params(dmi, param_kernel->data_size, *param_flags);

	return -EFAULT;
}