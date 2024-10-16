static long vhost_vdpa_get_status(struct vhost_vdpa *v, u8 __user *statusp)
{
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	u8 status;

	status = ops->get_status(vdpa);

	if (copy_to_user(statusp, &status, sizeof(status)))
		return -EFAULT;

	return 0;
}