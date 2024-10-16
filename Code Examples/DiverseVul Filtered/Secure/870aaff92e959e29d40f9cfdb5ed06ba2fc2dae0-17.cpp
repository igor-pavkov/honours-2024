static int vhost_vdpa_process_iotlb_update(struct vhost_vdpa *v,
					   struct vhost_iotlb_msg *msg)
{
	struct vhost_dev *dev = &v->vdev;
	struct vdpa_device *vdpa = v->vdpa;
	struct vhost_iotlb *iotlb = dev->iotlb;

	if (msg->iova < v->range.first || !msg->size ||
	    msg->iova > U64_MAX - msg->size + 1 ||
	    msg->iova + msg->size - 1 > v->range.last)
		return -EINVAL;

	if (vhost_iotlb_itree_first(iotlb, msg->iova,
				    msg->iova + msg->size - 1))
		return -EEXIST;

	if (vdpa->use_va)
		return vhost_vdpa_va_map(v, msg->iova, msg->size,
					 msg->uaddr, msg->perm);

	return vhost_vdpa_pa_map(v, msg->iova, msg->size, msg->uaddr,
				 msg->perm);
}