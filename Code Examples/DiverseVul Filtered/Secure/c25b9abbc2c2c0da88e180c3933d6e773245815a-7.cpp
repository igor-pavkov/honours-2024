static int skfp_send_pkt(struct sk_buff *skb, struct net_device *dev)
{
	struct s_smc *smc = netdev_priv(dev);
	skfddi_priv *bp = &smc->os;

	PRINTK(KERN_INFO "skfp_send_pkt\n");

	/*
	 * Verify that incoming transmit request is OK
	 *
	 * Note: The packet size check is consistent with other
	 *               Linux device drivers, although the correct packet
	 *               size should be verified before calling the
	 *               transmit routine.
	 */

	if (!(skb->len >= FDDI_K_LLC_ZLEN && skb->len <= FDDI_K_LLC_LEN)) {
		bp->MacStat.gen.tx_errors++;	/* bump error counter */
		// dequeue packets from xmt queue and send them
		netif_start_queue(dev);
		dev_kfree_skb(skb);
		return (0);	/* return "success" */
	}
	if (bp->QueueSkb == 0) {	// return with tbusy set: queue full

		netif_stop_queue(dev);
		return 1;
	}
	bp->QueueSkb--;
	skb_queue_tail(&bp->SendSkbQueue, skb);
	send_queued_packets(netdev_priv(dev));
	if (bp->QueueSkb == 0) {
		netif_stop_queue(dev);
	}
	dev->trans_start = jiffies;
	return 0;

}				// skfp_send_pkt