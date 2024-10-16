static void vhost_vdpa_unsetup_vq_irq(struct vhost_vdpa *v, u16 qid)
{
	struct vhost_virtqueue *vq = &v->vqs[qid];

	irq_bypass_unregister_producer(&vq->call_ctx.producer);
}