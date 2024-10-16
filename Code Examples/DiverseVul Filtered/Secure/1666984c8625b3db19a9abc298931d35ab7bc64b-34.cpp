static void usbnet_terminate_urbs(struct usbnet *dev)
{
	DECLARE_WAITQUEUE(wait, current);
	int temp;

	/* ensure there are no more active urbs */
	add_wait_queue(&dev->wait, &wait);
	set_current_state(TASK_UNINTERRUPTIBLE);
	temp = unlink_urbs(dev, &dev->txq) +
		unlink_urbs(dev, &dev->rxq);

	/* maybe wait for deletions to finish. */
	wait_skb_queue_empty(&dev->rxq);
	wait_skb_queue_empty(&dev->txq);
	wait_skb_queue_empty(&dev->done);
	netif_dbg(dev, ifdown, dev->net,
		  "waited for %d urb completions\n", temp);
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&dev->wait, &wait);
}