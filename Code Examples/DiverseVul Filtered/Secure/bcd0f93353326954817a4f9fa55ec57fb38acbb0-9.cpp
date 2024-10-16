static void pep_sock_unhash(struct sock *sk)
{
	struct pep_sock *pn = pep_sk(sk);
	struct sock *skparent = NULL;

	lock_sock(sk);

	if (pn->listener != NULL) {
		skparent = pn->listener;
		pn->listener = NULL;
		release_sock(sk);

		pn = pep_sk(skparent);
		lock_sock(skparent);
		sk_del_node_init(sk);
		sk = skparent;
	}

	/* Unhash a listening sock only when it is closed
	 * and all of its active connected pipes are closed. */
	if (hlist_empty(&pn->hlist))
		pn_sock_unhash(&pn->pn_sk.sk);
	release_sock(sk);

	if (skparent)
		sock_put(skparent);
}