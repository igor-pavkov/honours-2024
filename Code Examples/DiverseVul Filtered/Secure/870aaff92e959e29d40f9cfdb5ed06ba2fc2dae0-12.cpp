static void vhost_vdpa_iotlb_unmap(struct vhost_vdpa *v, u64 start, u64 last)
{
	struct vdpa_device *vdpa = v->vdpa;

	if (vdpa->use_va)
		return vhost_vdpa_va_unmap(v, start, last);

	return vhost_vdpa_pa_unmap(v, start, last);
}