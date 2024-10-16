static void rpmsg_downref_sleepers(struct virtproc_info *vrp)
{
	/* support multiple concurrent senders */
	mutex_lock(&vrp->tx_lock);

	/* are we the last sleeping context waiting for tx buffers ? */
	if (atomic_dec_and_test(&vrp->sleepers))
		/* disable "tx-complete" interrupts */
		virtqueue_disable_cb(vrp->svq);

	mutex_unlock(&vrp->tx_lock);
}