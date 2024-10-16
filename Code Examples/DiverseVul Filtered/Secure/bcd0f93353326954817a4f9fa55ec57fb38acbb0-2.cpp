static void pep_sock_close(struct sock *sk, long timeout)
{
	struct pep_sock *pn = pep_sk(sk);
	int ifindex = 0;

	sock_hold(sk); /* keep a reference after sk_common_release() */
	sk_common_release(sk);

	lock_sock(sk);
	if ((1 << sk->sk_state) & (TCPF_SYN_RECV|TCPF_ESTABLISHED)) {
		if (sk->sk_backlog_rcv == pipe_do_rcv)
			/* Forcefully remove dangling Phonet pipe */
			pipe_do_remove(sk);
		else
			pipe_handler_request(sk, PNS_PEP_DISCONNECT_REQ, PAD,
						NULL, 0);
	}
	sk->sk_state = TCP_CLOSE;

	ifindex = pn->ifindex;
	pn->ifindex = 0;
	release_sock(sk);

	if (ifindex)
		gprs_detach(sk);
	sock_put(sk);
}