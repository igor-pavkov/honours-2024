static int zr364xx_got_frame(struct zr364xx_camera *cam, int jpgsize)
{
	struct zr364xx_dmaqueue *dma_q = &cam->vidq;
	struct zr364xx_buffer *buf;
	unsigned long flags = 0;
	int rc = 0;

	DBG("wakeup: %p\n", &dma_q);
	spin_lock_irqsave(&cam->slock, flags);

	if (list_empty(&dma_q->active)) {
		DBG("No active queue to serve\n");
		rc = -1;
		goto unlock;
	}
	buf = list_entry(dma_q->active.next,
			 struct zr364xx_buffer, vb.queue);

	if (!waitqueue_active(&buf->vb.done)) {
		/* no one active */
		rc = -1;
		goto unlock;
	}
	list_del(&buf->vb.queue);
	buf->vb.ts = ktime_get_ns();
	DBG("[%p/%d] wakeup\n", buf, buf->vb.i);
	zr364xx_fillbuff(cam, buf, jpgsize);
	wake_up(&buf->vb.done);
	DBG("wakeup [buf/i] [%p/%d]\n", buf, buf->vb.i);
unlock:
	spin_unlock_irqrestore(&cam->slock, flags);
	return rc;
}