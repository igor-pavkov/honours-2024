static int ctl_ioctl(struct file *file, uint command, struct dm_ioctl __user *user)
{
	int r = 0;
	int ioctl_flags;
	int param_flags;
	unsigned int cmd;
	struct dm_ioctl *param;
	ioctl_fn fn = NULL;
	size_t input_param_size;
	struct dm_ioctl param_kernel;

	/* only root can play with this */
	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;

	if (_IOC_TYPE(command) != DM_IOCTL)
		return -ENOTTY;

	cmd = _IOC_NR(command);

	/*
	 * Check the interface version passed in.  This also
	 * writes out the kernel's interface version.
	 */
	r = check_version(cmd, user);
	if (r)
		return r;

	/*
	 * Nothing more to do for the version command.
	 */
	if (cmd == DM_VERSION_CMD)
		return 0;

	fn = lookup_ioctl(cmd, &ioctl_flags);
	if (!fn) {
		DMWARN("dm_ctl_ioctl: unknown command 0x%x", command);
		return -ENOTTY;
	}

	/*
	 * Copy the parameters into kernel space.
	 */
	r = copy_params(user, &param_kernel, ioctl_flags, &param, &param_flags);

	if (r)
		return r;

	input_param_size = param->data_size;
	r = validate_params(cmd, param);
	if (r)
		goto out;

	param->data_size = offsetof(struct dm_ioctl, data);
	r = fn(file, param, input_param_size);

	if (unlikely(param->flags & DM_BUFFER_FULL_FLAG) &&
	    unlikely(ioctl_flags & IOCTL_FLAGS_NO_PARAMS))
		DMERR("ioctl %d tried to output some data but has IOCTL_FLAGS_NO_PARAMS set", cmd);

	if (!r && ioctl_flags & IOCTL_FLAGS_ISSUE_GLOBAL_EVENT)
		dm_issue_global_event();

	/*
	 * Copy the results back to userland.
	 */
	if (!r && copy_to_user(user, param, param->data_size))
		r = -EFAULT;

out:
	free_params(param, input_param_size, param_flags);
	return r;
}