static long vhost_vdpa_set_features(struct vhost_vdpa *v, u64 __user *featurep)
{
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	u64 features;

	/*
	 * It's not allowed to change the features after they have
	 * been negotiated.
	 */
	if (ops->get_status(vdpa) & VIRTIO_CONFIG_S_FEATURES_OK)
		return -EBUSY;

	if (copy_from_user(&features, featurep, sizeof(features)))
		return -EFAULT;

	if (vdpa_set_features(vdpa, features))
		return -EINVAL;

	return 0;
}