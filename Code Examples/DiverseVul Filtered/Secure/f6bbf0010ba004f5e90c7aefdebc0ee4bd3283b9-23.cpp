static void vhost_vdpa_release_dev(struct device *device)
{
	struct vhost_vdpa *v =
	       container_of(device, struct vhost_vdpa, dev);

	ida_simple_remove(&vhost_vdpa_ida, v->minor);
	kfree(v->vqs);
	kfree(v);
}