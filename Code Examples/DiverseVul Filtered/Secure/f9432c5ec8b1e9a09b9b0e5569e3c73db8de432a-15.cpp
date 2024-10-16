static void rfcomm_set_owner_w(struct sk_buff *skb, struct rfcomm_dev *dev)
{
	tty_port_get(&dev->port);
	atomic_add(skb->truesize, &dev->wmem_alloc);
	skb->sk = (void *) dev;
	skb->destructor = rfcomm_wfree;
}