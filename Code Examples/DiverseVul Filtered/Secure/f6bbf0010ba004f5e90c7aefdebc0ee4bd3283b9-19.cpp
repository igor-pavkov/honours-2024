static long vhost_vdpa_get_features(struct vhost_vdpa *v, u64 __user *featurep)
{
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	u64 features;

	features = ops->get_features(vdpa);

	if (copy_to_user(featurep, &features, sizeof(features)))
		return -EFAULT;

	return 0;
}