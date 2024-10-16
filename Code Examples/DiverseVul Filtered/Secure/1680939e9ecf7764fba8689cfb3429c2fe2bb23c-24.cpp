static void rpmsg_xmit_done(struct virtqueue *svq)
{
	struct virtproc_info *vrp = svq->vdev->priv;

	dev_dbg(&svq->vdev->dev, "%s\n", __func__);

	/* wake up potential senders that are waiting for a tx buffer */
	wake_up_interruptible(&vrp->sendq);
}