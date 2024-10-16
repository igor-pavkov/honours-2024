static int pipe_handler_send_created_ind(struct sock *sk)
{
	struct pep_sock *pn = pep_sk(sk);
	u8 data[4] = {
		PN_PIPE_SB_NEGOTIATED_FC, pep_sb_size(2),
		pn->tx_fc, pn->rx_fc,
	};

	return pep_indicate(sk, PNS_PIPE_CREATED_IND, 1 /* sub-blocks */,
				data, 4, GFP_ATOMIC);
}