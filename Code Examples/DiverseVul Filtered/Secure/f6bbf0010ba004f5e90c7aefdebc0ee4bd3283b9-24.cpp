static irqreturn_t vhost_vdpa_virtqueue_cb(void *private)
{
	struct vhost_virtqueue *vq = private;
	struct eventfd_ctx *call_ctx = vq->call_ctx.ctx;

	if (call_ctx)
		eventfd_signal(call_ctx, 1);

	return IRQ_HANDLED;
}