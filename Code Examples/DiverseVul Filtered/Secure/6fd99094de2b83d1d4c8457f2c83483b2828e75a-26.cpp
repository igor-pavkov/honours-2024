static void ndisc_fill_addr_option(struct sk_buff *skb, int type, void *data)
{
	int pad   = ndisc_addr_option_pad(skb->dev->type);
	int data_len = skb->dev->addr_len;
	int space = ndisc_opt_addr_space(skb->dev);
	u8 *opt = skb_put(skb, space);

	opt[0] = type;
	opt[1] = space>>3;

	memset(opt + 2, 0, pad);
	opt   += pad;
	space -= pad;

	memcpy(opt+2, data, data_len);
	data_len += 2;
	opt += data_len;
	space -= data_len;
	if (space > 0)
		memset(opt, 0, space);
}