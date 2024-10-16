static int pipe_rcv_status(struct sock *sk, struct sk_buff *skb)
{
	struct pep_sock *pn = pep_sk(sk);
	struct pnpipehdr *hdr;
	int wake = 0;

	if (!pskb_may_pull(skb, sizeof(*hdr) + 4))
		return -EINVAL;

	hdr = pnp_hdr(skb);
	if (hdr->pep_type != PN_PEP_TYPE_COMMON) {
		net_dbg_ratelimited("Phonet unknown PEP type: %u\n",
				    (unsigned int)hdr->pep_type);
		return -EOPNOTSUPP;
	}

	switch (hdr->data[0]) {
	case PN_PEP_IND_FLOW_CONTROL:
		switch (pn->tx_fc) {
		case PN_LEGACY_FLOW_CONTROL:
			switch (hdr->data[3]) {
			case PEP_IND_BUSY:
				atomic_set(&pn->tx_credits, 0);
				break;
			case PEP_IND_READY:
				atomic_set(&pn->tx_credits, wake = 1);
				break;
			}
			break;
		case PN_ONE_CREDIT_FLOW_CONTROL:
			if (hdr->data[3] == PEP_IND_READY)
				atomic_set(&pn->tx_credits, wake = 1);
			break;
		}
		break;

	case PN_PEP_IND_ID_MCFC_GRANT_CREDITS:
		if (pn->tx_fc != PN_MULTI_CREDIT_FLOW_CONTROL)
			break;
		atomic_add(wake = hdr->data[3], &pn->tx_credits);
		break;

	default:
		net_dbg_ratelimited("Phonet unknown PEP indication: %u\n",
				    (unsigned int)hdr->data[0]);
		return -EOPNOTSUPP;
	}
	if (wake)
		sk->sk_write_space(sk);
	return 0;
}