static long vhost_vdpa_get_vring_num(struct vhost_vdpa *v, u16 __user *argp)
{
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	u16 num;

	num = ops->get_vq_num_max(vdpa);

	if (copy_to_user(argp, &num, sizeof(num)))
		return -EFAULT;

	return 0;
}