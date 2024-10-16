static int llc_ui_connect(struct socket *sock, struct sockaddr *uaddr,
			  int addrlen, int flags)
{
	struct sock *sk = sock->sk;
	struct llc_sock *llc = llc_sk(sk);
	struct sockaddr_llc *addr = (struct sockaddr_llc *)uaddr;
	int rc = -EINVAL;

	lock_sock(sk);
	if (unlikely(addrlen != sizeof(*addr)))
		goto out;
	rc = -EAFNOSUPPORT;
	if (unlikely(addr->sllc_family != AF_LLC))
		goto out;
	if (unlikely(sk->sk_type != SOCK_STREAM))
		goto out;
	rc = -EALREADY;
	if (unlikely(sock->state == SS_CONNECTING))
		goto out;
	/* bind connection to sap if user hasn't done it. */
	if (sock_flag(sk, SOCK_ZAPPED)) {
		/* bind to sap with null dev, exclusive */
		rc = llc_ui_autobind(sock, addr);
		if (rc)
			goto out;
	}
	llc->daddr.lsap = addr->sllc_sap;
	memcpy(llc->daddr.mac, addr->sllc_mac, IFHWADDRLEN);
	sock->state = SS_CONNECTING;
	sk->sk_state   = TCP_SYN_SENT;
	llc->link   = llc_ui_next_link_no(llc->sap->laddr.lsap);
	rc = llc_establish_connection(sk, llc->dev->dev_addr,
				      addr->sllc_mac, addr->sllc_sap);
	if (rc) {
		dprintk("%s: llc_ui_send_conn failed :-(\n", __func__);
		sock->state  = SS_UNCONNECTED;
		sk->sk_state = TCP_CLOSE;
		goto out;
	}

	if (sk->sk_state == TCP_SYN_SENT) {
		const long timeo = sock_sndtimeo(sk, flags & O_NONBLOCK);

		if (!timeo || !llc_ui_wait_for_conn(sk, timeo))
			goto out;

		rc = sock_intr_errno(timeo);
		if (signal_pending(current))
			goto out;
	}

	if (sk->sk_state == TCP_CLOSE)
		goto sock_error;

	sock->state = SS_CONNECTED;
	rc = 0;
out:
	release_sock(sk);
	return rc;
sock_error:
	rc = sock_error(sk) ? : -ECONNABORTED;
	sock->state = SS_UNCONNECTED;
	goto out;
}