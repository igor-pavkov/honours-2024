static netdev_tx_t ax_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct mkiss *ax = netdev_priv(dev);

	if (skb->protocol == htons(ETH_P_IP))
		return ax25_ip_xmit(skb);

	if (!netif_running(dev))  {
		printk(KERN_ERR "mkiss: %s: xmit call when iface is down\n", dev->name);
		return NETDEV_TX_BUSY;
	}

	if (netif_queue_stopped(dev)) {
		/*
		 * May be we must check transmitter timeout here ?
		 *      14 Oct 1994 Dmitry Gorodchanin.
		 */
		if (time_before(jiffies, dev_trans_start(dev) + 20 * HZ)) {
			/* 20 sec timeout not reached */
			return NETDEV_TX_BUSY;
		}

		printk(KERN_ERR "mkiss: %s: transmit timed out, %s?\n", dev->name,
		       (tty_chars_in_buffer(ax->tty) || ax->xleft) ?
		       "bad line quality" : "driver error");

		ax->xleft = 0;
		clear_bit(TTY_DO_WRITE_WAKEUP, &ax->tty->flags);
		netif_start_queue(dev);
	}

	/* We were not busy, so we are now... :-) */
	netif_stop_queue(dev);
	ax_encaps(dev, skb->data, skb->len);
	kfree_skb(skb);

	return NETDEV_TX_OK;
}