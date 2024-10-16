static int vhost_vdpa_alloc_domain(struct vhost_vdpa *v)
{
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	struct device *dma_dev = vdpa_get_dma_dev(vdpa);
	struct bus_type *bus;
	int ret;

	/* Device want to do DMA by itself */
	if (ops->set_map || ops->dma_map)
		return 0;

	bus = dma_dev->bus;
	if (!bus)
		return -EFAULT;

	if (!iommu_capable(bus, IOMMU_CAP_CACHE_COHERENCY))
		return -ENOTSUPP;

	v->domain = iommu_domain_alloc(bus);
	if (!v->domain)
		return -EIO;

	ret = iommu_attach_device(v->domain, dma_dev);
	if (ret)
		goto err_attach;

	return 0;

err_attach:
	iommu_domain_free(v->domain);
	return ret;
}