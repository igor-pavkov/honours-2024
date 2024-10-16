int pep_writeable(struct sock *sk)
{
	struct pep_sock *pn = pep_sk(sk);

	return atomic_read(&pn->tx_credits);
}