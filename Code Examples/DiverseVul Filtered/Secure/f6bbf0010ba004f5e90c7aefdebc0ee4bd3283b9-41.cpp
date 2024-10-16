static void handle_vq_kick(struct vhost_work *work)
{
	struct vhost_virtqueue *vq = container_of(work, struct vhost_virtqueue,
						  poll.work);
	struct vhost_vdpa *v = container_of(vq->dev, struct vhost_vdpa, vdev);
	const struct vdpa_config_ops *ops = v->vdpa->config;

	ops->kick_vq(v->vdpa, vq - v->vqs);
}