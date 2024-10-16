static int pipe_rcv_created(struct sock *sk, struct sk_buff *skb)
{
	struct pep_sock *pn = pep_sk(sk);
	struct pnpipehdr *hdr = pnp_hdr(skb);
	u8 n_sb = hdr->data0;

	pn->rx_fc = pn->tx_fc = PN_LEGACY_FLOW_CONTROL;
	__skb_pull(skb, sizeof(*hdr));
	while (n_sb > 0) {
		u8 type, buf[2], len = sizeof(buf);
		u8 *data = pep_get_sb(skb, &type, &len, buf);

		if (data == NULL)
			return -EINVAL;
		switch (type) {
		case PN_PIPE_SB_NEGOTIATED_FC:
			if (len < 2 || (data[0] | data[1]) > 3)
				break;
			pn->tx_fc = data[0] & 3;
			pn->rx_fc = data[1] & 3;
			break;
		}
		n_sb--;
	}
	return 0;
}