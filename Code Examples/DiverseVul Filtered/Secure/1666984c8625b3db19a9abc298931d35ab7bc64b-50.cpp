static int __init usbnet_init(void)
{
	/* Compiler should optimize this out. */
	BUILD_BUG_ON(
		FIELD_SIZEOF(struct sk_buff, cb) < sizeof(struct skb_data));

	eth_random_addr(node_id);
	return 0;
}