static void vhost_vdpa_setup_vq_irq(struct vhost_vdpa *v, u16 qid)
{
	struct vhost_virtqueue *vq = &v->vqs[qid];
	const struct vdpa_config_ops *ops = v->vdpa->config;
	struct vdpa_device *vdpa = v->vdpa;
	int ret, irq;

	if (!ops->get_vq_irq)
		return;

	irq = ops->get_vq_irq(vdpa, qid);
	irq_bypass_unregister_producer(&vq->call_ctx.producer);
	if (!vq->call_ctx.ctx || irq < 0)
		return;

	vq->call_ctx.producer.token = vq->call_ctx.ctx;
	vq->call_ctx.producer.irq = irq;
	ret = irq_bypass_register_producer(&vq->call_ctx.producer);
	if (unlikely(ret))
		dev_info(&v->dev, "vq %u, irq bypass producer (token %p) registration fails, ret =  %d\n",
			 qid, vq->call_ctx.producer.token, ret);
}