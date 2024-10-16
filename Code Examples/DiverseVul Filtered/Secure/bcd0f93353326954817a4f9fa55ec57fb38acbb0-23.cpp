static struct sock *pep_find_pipe(const struct hlist_head *hlist,
					const struct sockaddr_pn *dst,
					u8 pipe_handle)
{
	struct sock *sknode;
	u16 dobj = pn_sockaddr_get_object(dst);

	sk_for_each(sknode, hlist) {
		struct pep_sock *pnnode = pep_sk(sknode);

		/* Ports match, but addresses might not: */
		if (pnnode->pn_sk.sobject != dobj)
			continue;
		if (pnnode->pipe_handle != pipe_handle)
			continue;
		if (sknode->sk_state == TCP_CLOSE_WAIT)
			continue;

		sock_hold(sknode);
		return sknode;
	}
	return NULL;
}