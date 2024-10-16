static long vhost_vdpa_unlocked_ioctl(struct file *filep,
				      unsigned int cmd, unsigned long arg)
{
	struct vhost_vdpa *v = filep->private_data;
	struct vhost_dev *d = &v->vdev;
	void __user *argp = (void __user *)arg;
	u64 __user *featurep = argp;
	u64 features;
	long r = 0;

	if (cmd == VHOST_SET_BACKEND_FEATURES) {
		if (copy_from_user(&features, featurep, sizeof(features)))
			return -EFAULT;
		if (features & ~VHOST_VDPA_BACKEND_FEATURES)
			return -EOPNOTSUPP;
		vhost_set_backend_features(&v->vdev, features);
		return 0;
	}

	mutex_lock(&d->mutex);

	switch (cmd) {
	case VHOST_VDPA_GET_DEVICE_ID:
		r = vhost_vdpa_get_device_id(v, argp);
		break;
	case VHOST_VDPA_GET_STATUS:
		r = vhost_vdpa_get_status(v, argp);
		break;
	case VHOST_VDPA_SET_STATUS:
		r = vhost_vdpa_set_status(v, argp);
		break;
	case VHOST_VDPA_GET_CONFIG:
		r = vhost_vdpa_get_config(v, argp);
		break;
	case VHOST_VDPA_SET_CONFIG:
		r = vhost_vdpa_set_config(v, argp);
		break;
	case VHOST_GET_FEATURES:
		r = vhost_vdpa_get_features(v, argp);
		break;
	case VHOST_SET_FEATURES:
		r = vhost_vdpa_set_features(v, argp);
		break;
	case VHOST_VDPA_GET_VRING_NUM:
		r = vhost_vdpa_get_vring_num(v, argp);
		break;
	case VHOST_SET_LOG_BASE:
	case VHOST_SET_LOG_FD:
		r = -ENOIOCTLCMD;
		break;
	case VHOST_VDPA_SET_CONFIG_CALL:
		r = vhost_vdpa_set_config_call(v, argp);
		break;
	case VHOST_GET_BACKEND_FEATURES:
		features = VHOST_VDPA_BACKEND_FEATURES;
		if (copy_to_user(featurep, &features, sizeof(features)))
			r = -EFAULT;
		break;
	case VHOST_VDPA_GET_IOVA_RANGE:
		r = vhost_vdpa_get_iova_range(v, argp);
		break;
	default:
		r = vhost_dev_ioctl(&v->vdev, cmd, argp);
		if (r == -ENOIOCTLCMD)
			r = vhost_vdpa_vring_ioctl(v, cmd, argp);
		break;
	}

	mutex_unlock(&d->mutex);
	return r;
}