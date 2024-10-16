static int pep_connresp_rcv(struct sock *sk, struct sk_buff *skb)
{
	struct pep_sock *pn = pep_sk(sk);
	struct pnpipehdr *hdr;
	u8 n_sb;

	if (!pskb_pull(skb, sizeof(*hdr) + 4))
		return -EINVAL;

	hdr = pnp_hdr(skb);
	if (hdr->error_code != PN_PIPE_NO_ERROR)
		return -ECONNREFUSED;

	/* Parse sub-blocks */
	n_sb = hdr->data[3];
	while (n_sb > 0) {
		u8 type, buf[6], len = sizeof(buf);
		const u8 *data = pep_get_sb(skb, &type, &len, buf);

		if (data == NULL)
			return -EINVAL;

		switch (type) {
		case PN_PIPE_SB_REQUIRED_FC_TX:
			if (len < 2 || len < data[0])
				break;
			pn->tx_fc = pipe_negotiate_fc(data + 2, len - 2);
			break;

		case PN_PIPE_SB_PREFERRED_FC_RX:
			if (len < 2 || len < data[0])
				break;
			pn->rx_fc = pipe_negotiate_fc(data + 2, len - 2);
			break;

		}
		n_sb--;
	}

	return pipe_handler_send_created_ind(sk);
}