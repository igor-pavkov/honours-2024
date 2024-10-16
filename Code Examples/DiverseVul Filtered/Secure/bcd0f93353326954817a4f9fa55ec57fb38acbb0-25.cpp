static int pep_reject_conn(struct sock *sk, struct sk_buff *skb, u8 code,
				gfp_t priority)
{
	static const u8 data[4] = { PAD, PAD, PAD, 0 /* sub-blocks */ };
	WARN_ON(code == PN_PIPE_NO_ERROR);
	return pep_reply(sk, skb, code, data, sizeof(data), priority);
}