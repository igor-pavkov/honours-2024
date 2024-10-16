static int __init mod_init(void)
{
	skb_queue_head_init(&tx_queue);
	register_hdlc_protocol(&proto);
	return 0;
}