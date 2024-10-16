static int vhost_vdpa_reset(struct vhost_vdpa *v)
{
	struct vdpa_device *vdpa = v->vdpa;

	v->in_batch = 0;

	return vdpa_reset(vdpa);
}