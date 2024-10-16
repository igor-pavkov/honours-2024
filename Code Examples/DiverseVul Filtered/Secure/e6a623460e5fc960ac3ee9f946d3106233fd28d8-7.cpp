static long media_device_enum_links32(struct media_device *mdev,
				      struct media_links_enum32 __user *ulinks)
{
	struct media_links_enum links;
	compat_uptr_t pads_ptr, links_ptr;

	memset(&links, 0, sizeof(links));

	if (get_user(links.entity, &ulinks->entity)
	    || get_user(pads_ptr, &ulinks->pads)
	    || get_user(links_ptr, &ulinks->links))
		return -EFAULT;

	links.pads = compat_ptr(pads_ptr);
	links.links = compat_ptr(links_ptr);

	return __media_device_enum_links(mdev, &links);
}