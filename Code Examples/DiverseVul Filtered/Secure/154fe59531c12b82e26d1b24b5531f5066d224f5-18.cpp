void vq_clear_used_ring_flags(struct virtio_base *base, struct virtio_vq_info *vq)
{
	int backend_type = base->backend_type;
	int polling_in_progress = base->polling_in_progress;

	/* we should never unmask notification in polling mode */
	if (virtio_poll_enabled && backend_type == BACKEND_VBSU && polling_in_progress == 1)
		return;

	vq->used->flags &= ~VRING_USED_F_NO_NOTIFY;
}