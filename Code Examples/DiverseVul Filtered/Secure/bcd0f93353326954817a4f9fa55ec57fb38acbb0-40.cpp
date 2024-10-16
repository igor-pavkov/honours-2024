static int pep_ctrlreq_error(struct sock *sk, struct sk_buff *oskb, u8 code,
				gfp_t priority)
{
	const struct pnpipehdr *oph = pnp_hdr(oskb);
	struct sk_buff *skb;
	struct pnpipehdr *ph;
	struct sockaddr_pn dst;
	u8 data[4] = {
		oph->pep_type, /* PEP type */
		code, /* error code, at an unusual offset */
		PAD, PAD,
	};

	skb = pep_alloc_skb(sk, data, 4, priority);
	if (!skb)
		return -ENOMEM;

	ph = pnp_hdr(skb);
	ph->utid = oph->utid;
	ph->message_id = PNS_PEP_CTRL_RESP;
	ph->pipe_handle = oph->pipe_handle;
	ph->data0 = oph->data[0]; /* CTRL id */

	pn_skb_get_src_sockaddr(oskb, &dst);
	return pn_skb_send(sk, skb, &dst);
}