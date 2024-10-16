static void rfcomm_dev_data_ready(struct rfcomm_dlc *dlc, struct sk_buff *skb)
{
	struct rfcomm_dev *dev = dlc->owner;
	struct tty_struct *tty;

	if (!dev) {
		kfree_skb(skb);
		return;
	}

	tty = dev->port.tty;
	if (!tty || !skb_queue_empty(&dev->pending)) {
		skb_queue_tail(&dev->pending, skb);
		return;
	}

	BT_DBG("dlc %p tty %p len %d", dlc, tty, skb->len);

	tty_insert_flip_string(tty, skb->data, skb->len);
	tty_flip_buffer_push(tty);

	kfree_skb(skb);
}