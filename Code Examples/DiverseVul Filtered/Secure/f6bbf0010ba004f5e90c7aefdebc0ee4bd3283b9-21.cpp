static long vhost_vdpa_get_config(struct vhost_vdpa *v,
				  struct vhost_vdpa_config __user *c)
{
	struct vdpa_device *vdpa = v->vdpa;
	struct vhost_vdpa_config config;
	unsigned long size = offsetof(struct vhost_vdpa_config, buf);
	u8 *buf;

	if (copy_from_user(&config, c, size))
		return -EFAULT;
	if (vhost_vdpa_config_validate(v, &config))
		return -EINVAL;
	buf = kvzalloc(config.len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	vdpa_get_config(vdpa, config.off, buf, config.len);

	if (copy_to_user(c->buf, buf, config.len)) {
		kvfree(buf);
		return -EFAULT;
	}

	kvfree(buf);
	return 0;
}