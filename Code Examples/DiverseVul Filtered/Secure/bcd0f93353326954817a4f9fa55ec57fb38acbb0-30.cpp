static void pipe_grant_credits(struct sock *sk, gfp_t priority)
{
	struct pep_sock *pn = pep_sk(sk);

	BUG_ON(sk->sk_state != TCP_ESTABLISHED);

	switch (pn->rx_fc) {
	case PN_LEGACY_FLOW_CONTROL: /* TODO */
		break;
	case PN_ONE_CREDIT_FLOW_CONTROL:
		if (pipe_snd_status(sk, PN_PEP_IND_FLOW_CONTROL,
					PEP_IND_READY, priority) == 0)
			pn->rx_credits = 1;
		break;
	case PN_MULTI_CREDIT_FLOW_CONTROL:
		if ((pn->rx_credits + CREDITS_THR) > CREDITS_MAX)
			break;
		if (pipe_snd_status(sk, PN_PEP_IND_ID_MCFC_GRANT_CREDITS,
					CREDITS_MAX - pn->rx_credits,
					priority) == 0)
			pn->rx_credits = CREDITS_MAX;
		break;
	}
}