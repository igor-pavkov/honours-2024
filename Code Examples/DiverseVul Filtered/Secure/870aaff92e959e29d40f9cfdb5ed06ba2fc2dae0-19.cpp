static void vhost_vdpa_set_iova_range(struct vhost_vdpa *v)
{
	struct vdpa_iova_range *range = &v->range;
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;

	if (ops->get_iova_range) {
		*range = ops->get_iova_range(vdpa);
	} else if (v->domain && v->domain->geometry.force_aperture) {
		range->first = v->domain->geometry.aperture_start;
		range->last = v->domain->geometry.aperture_end;
	} else {
		range->first = 0;
		range->last = ULLONG_MAX;
	}
}