int mac_drv_rx_init(struct s_smc *smc, int len, int fc,
		    char *look_ahead, int la_len)
{
	struct sk_buff *skb;

	PRINTK("entering mac_drv_rx_init(len=%d)\n", len);

	// "Received" a SMT or NSA frame of the local SMT.

	if (len != la_len || len < FDDI_MAC_HDR_LEN || !look_ahead) {
		PRINTK("fddi: Discard invalid local SMT frame\n");
		PRINTK("  len=%d, la_len=%d, (ULONG) look_ahead=%08lXh.\n",
		       len, la_len, (unsigned long) look_ahead);
		return (0);
	}
	skb = alloc_skb(len + 3, GFP_ATOMIC);
	if (!skb) {
		PRINTK("fddi: Local SMT: skb memory exhausted.\n");
		return (0);
	}
	skb_reserve(skb, 3);
	skb_put(skb, len);
	skb_copy_to_linear_data(skb, look_ahead, len);

	// deliver frame to system
	skb->protocol = fddi_type_trans(skb, smc->os.dev);
	netif_rx(skb);

	return (0);
}				// mac_drv_rx_init