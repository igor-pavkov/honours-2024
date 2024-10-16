static void rfcomm_wfree(struct sk_buff *skb)
{
	struct rfcomm_dev *dev = (void *) skb->sk;
	struct tty_struct *tty = dev->port.tty;
	atomic_sub(skb->truesize, &dev->wmem_alloc);
	if (test_bit(RFCOMM_TTY_ATTACHED, &dev->flags) && tty)
		tty_wakeup(tty);
	tty_port_put(&dev->port);
}