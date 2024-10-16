static int vhost_net_tx_get_vq_desc(struct vhost_net *net,
				    struct vhost_virtqueue *vq,
				    struct iovec iov[], unsigned int iov_size,
				    unsigned int *out_num, unsigned int *in_num)
{
	unsigned long uninitialized_var(endtime);
	int r = vhost_get_vq_desc(vq, vq->iov, ARRAY_SIZE(vq->iov),
				  out_num, in_num, NULL, NULL);

	if (r == vq->num && vq->busyloop_timeout) {
		preempt_disable();
		endtime = busy_clock() + vq->busyloop_timeout;
		while (vhost_can_busy_poll(vq->dev, endtime) &&
		       vhost_vq_avail_empty(vq->dev, vq))
			cpu_relax();
		preempt_enable();
		r = vhost_get_vq_desc(vq, vq->iov, ARRAY_SIZE(vq->iov),
				      out_num, in_num, NULL, NULL);
	}

	return r;
}