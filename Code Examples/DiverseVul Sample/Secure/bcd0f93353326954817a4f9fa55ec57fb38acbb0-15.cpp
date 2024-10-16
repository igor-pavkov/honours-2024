static unsigned char *pep_get_sb(struct sk_buff *skb, u8 *ptype, u8 *plen,
					void *buf)
{
	void *data = NULL;
	struct {
		u8 sb_type;
		u8 sb_len;
	} *ph, h;
	int buflen = *plen;

	ph = skb_header_pointer(skb, 0, 2, &h);
	if (ph == NULL || ph->sb_len < 2 || !pskb_may_pull(skb, ph->sb_len))
		return NULL;
	ph->sb_len -= 2;
	*ptype = ph->sb_type;
	*plen = ph->sb_len;

	if (buflen > ph->sb_len)
		buflen = ph->sb_len;
	data = skb_header_pointer(skb, 2, buflen, buf);
	__skb_pull(skb, 2 + ph->sb_len);
	return data;
}