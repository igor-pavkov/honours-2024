static void pipe_start_flow_control(struct sock *sk)
{
	struct pep_sock *pn = pep_sk(sk);

	if (!pn_flow_safe(pn->tx_fc)) {
		atomic_set(&pn->tx_credits, 1);
		sk->sk_write_space(sk);
	}
	pipe_grant_credits(sk, GFP_ATOMIC);
}