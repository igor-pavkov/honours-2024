static void sp_bump(struct sixpack *sp, char cmd)
{
	struct sk_buff *skb;
	int count;
	unsigned char *ptr;

	count = sp->rcount + 1;

	sp->dev->stats.rx_bytes += count;

	if ((skb = dev_alloc_skb(count + 1)) == NULL)
		goto out_mem;

	ptr = skb_put(skb, count + 1);
	*ptr++ = cmd;	/* KISS command */

	memcpy(ptr, sp->cooked_buf + 1, count);
	skb->protocol = ax25_type_trans(skb, sp->dev);
	netif_rx(skb);
	sp->dev->stats.rx_packets++;

	return;

out_mem:
	sp->dev->stats.rx_dropped++;
}