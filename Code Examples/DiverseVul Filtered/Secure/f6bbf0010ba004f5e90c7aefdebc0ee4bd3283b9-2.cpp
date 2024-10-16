static void vhost_vdpa_unmap(struct vhost_vdpa *v, u64 iova, u64 size)
{
	struct vhost_dev *dev = &v->vdev;
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;

	vhost_vdpa_iotlb_unmap(v, iova, iova + size - 1);

	if (ops->dma_map) {
		ops->dma_unmap(vdpa, iova, size);
	} else if (ops->set_map) {
		if (!v->in_batch)
			ops->set_map(vdpa, dev->iotlb);
	} else {
		iommu_unmap(v->domain, iova, size);
	}
}