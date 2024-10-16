static void *get_a_tx_buf(struct virtproc_info *vrp)
{
	unsigned int len;
	void *ret;

	/* support multiple concurrent senders */
	mutex_lock(&vrp->tx_lock);

	/*
	 * either pick the next unused tx buffer
	 * (half of our buffers are used for sending messages)
	 */
	if (vrp->last_sbuf < vrp->num_bufs / 2)
		ret = vrp->sbufs + vrp->buf_size * vrp->last_sbuf++;
	/* or recycle a used one */
	else
		ret = virtqueue_get_buf(vrp->svq, &len);

	mutex_unlock(&vrp->tx_lock);

	return ret;
}