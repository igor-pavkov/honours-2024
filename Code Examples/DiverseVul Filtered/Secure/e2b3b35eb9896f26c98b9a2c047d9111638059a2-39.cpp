static bool vhost_exceeds_maxpend(struct vhost_net *net)
{
	struct vhost_net_virtqueue *nvq = &net->vqs[VHOST_NET_VQ_TX];
	struct vhost_virtqueue *vq = &nvq->vq;

	return (nvq->upend_idx + UIO_MAXIOV - nvq->done_idx) % UIO_MAXIOV >
	       min_t(unsigned int, VHOST_MAX_PEND, vq->num >> 2);
}