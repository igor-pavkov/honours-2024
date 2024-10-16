static void ppp_tx_cp(struct net_device *dev, u16 pid, u8 code,
		      u8 id, unsigned int len, const void *data)
{
	struct sk_buff *skb;
	struct cp_header *cp;
	unsigned int magic_len = 0;
	static u32 magic;

#if DEBUG_CP
	int i;
	char *ptr;
#endif

	if (pid == PID_LCP && (code == LCP_ECHO_REQ || code == LCP_ECHO_REPLY))
		magic_len = sizeof(magic);

	skb = dev_alloc_skb(sizeof(struct hdlc_header) +
			    sizeof(struct cp_header) + magic_len + len);
	if (!skb) {
		netdev_warn(dev, "out of memory in ppp_tx_cp()\n");
		return;
	}
	skb_reserve(skb, sizeof(struct hdlc_header));

	cp = skb_put(skb, sizeof(struct cp_header));
	cp->code = code;
	cp->id = id;
	cp->len = htons(sizeof(struct cp_header) + magic_len + len);

	if (magic_len)
		skb_put_data(skb, &magic, magic_len);
	if (len)
		skb_put_data(skb, data, len);

#if DEBUG_CP
	BUG_ON(code >= CP_CODES);
	ptr = debug_buffer;
	*ptr = '\x0';
	for (i = 0; i < min_t(unsigned int, magic_len + len, DEBUG_CP); i++) {
		sprintf(ptr, " %02X", skb->data[sizeof(struct cp_header) + i]);
		ptr += strlen(ptr);
	}
	printk(KERN_DEBUG "%s: TX %s [%s id 0x%X]%s\n", dev->name,
	       proto_name(pid), code_names[code], id, debug_buffer);
#endif

	ppp_hard_header(skb, dev, pid, NULL, NULL, 0);

	skb->priority = TC_PRIO_CONTROL;
	skb->dev = dev;
	skb_reset_network_header(skb);
	skb_queue_tail(&tx_queue, skb);
}