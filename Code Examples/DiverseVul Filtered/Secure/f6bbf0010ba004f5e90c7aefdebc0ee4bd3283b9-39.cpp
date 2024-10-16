static void vhost_vdpa_free_domain(struct vhost_vdpa *v)
{
	struct vdpa_device *vdpa = v->vdpa;
	struct device *dma_dev = vdpa_get_dma_dev(vdpa);

	if (v->domain) {
		iommu_detach_device(v->domain, dma_dev);
		iommu_domain_free(v->domain);
	}

	v->domain = NULL;
}