static void vhost_vdpa_iotlb_free(struct vhost_vdpa *v)
{
	struct vhost_dev *dev = &v->vdev;

	vhost_vdpa_iotlb_unmap(v, 0ULL, 0ULL - 1);
	kfree(dev->iotlb);
	dev->iotlb = NULL;
}