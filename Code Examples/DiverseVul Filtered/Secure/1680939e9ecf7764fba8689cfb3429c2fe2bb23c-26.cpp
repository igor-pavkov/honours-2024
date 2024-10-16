static void rpmsg_upref_sleepers(struct virtproc_info *vrp)
{
	/* support multiple concurrent senders */
	mutex_lock(&vrp->tx_lock);

	/* are we the first sleeping context waiting for tx buffers ? */
	if (atomic_inc_return(&vrp->sleepers) == 1)
		/* enable "tx-complete" interrupts before dozing off */
		virtqueue_enable_cb(vrp->svq);

	mutex_unlock(&vrp->tx_lock);
}