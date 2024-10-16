static void __tun_detach(struct tun_struct *tun)
{
	/* Detach from net device */
	netif_tx_lock_bh(tun->dev);
	tun->tfile = NULL;
	netif_tx_unlock_bh(tun->dev);

	/* Drop read queue */
	skb_queue_purge(&tun->readq);

	/* Drop the extra count on the net device */
	dev_put(tun->dev);
}