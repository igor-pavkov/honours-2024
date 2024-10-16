static long vhost_vdpa_get_iova_range(struct vhost_vdpa *v, u32 __user *argp)
{
	struct vhost_vdpa_iova_range range = {
		.first = v->range.first,
		.last = v->range.last,
	};

	if (copy_to_user(argp, &range, sizeof(range)))
		return -EFAULT;
	return 0;
}