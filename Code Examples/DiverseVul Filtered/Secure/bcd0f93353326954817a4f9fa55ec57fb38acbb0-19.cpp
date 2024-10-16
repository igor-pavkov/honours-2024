static int pep_enableresp_rcv(struct sock *sk, struct sk_buff *skb)
{
	struct pnpipehdr *hdr = pnp_hdr(skb);

	if (hdr->error_code != PN_PIPE_NO_ERROR)
		return -ECONNREFUSED;

	return pep_indicate(sk, PNS_PIPE_ENABLED_IND, 0 /* sub-blocks */,
		NULL, 0, GFP_ATOMIC);

}