static long vhost_vdpa_get_device_id(struct vhost_vdpa *v, u8 __user *argp)
{
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	u32 device_id;

	device_id = ops->get_device_id(vdpa);

	if (copy_to_user(argp, &device_id, sizeof(device_id)))
		return -EFAULT;

	return 0;
}