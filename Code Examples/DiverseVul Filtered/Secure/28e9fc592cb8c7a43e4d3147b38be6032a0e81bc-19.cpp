static int llc_ui_shutdown(struct socket *sock, int how)
{
	struct sock *sk = sock->sk;
	int rc = -ENOTCONN;

	lock_sock(sk);
	if (unlikely(sk->sk_state != TCP_ESTABLISHED))
		goto out;
	rc = -EINVAL;
	if (how != 2)
		goto out;
	rc = llc_send_disc(sk);
	if (!rc)
		rc = llc_ui_wait_for_disc(sk, sk->sk_rcvtimeo);
	/* Wake up anyone sleeping in poll */
	sk->sk_state_change(sk);
out:
	release_sock(sk);
	return rc;
}