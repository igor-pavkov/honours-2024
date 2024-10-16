static int llc_ui_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct llc_sock *llc;

	if (unlikely(sk == NULL))
		goto out;
	sock_hold(sk);
	lock_sock(sk);
	llc = llc_sk(sk);
	dprintk("%s: closing local(%02X) remote(%02X)\n", __func__,
		llc->laddr.lsap, llc->daddr.lsap);
	if (!llc_send_disc(sk))
		llc_ui_wait_for_disc(sk, sk->sk_rcvtimeo);
	if (!sock_flag(sk, SOCK_ZAPPED)) {
		llc_sap_put(llc->sap);
		llc_sap_remove_socket(llc->sap, sk);
	}
	release_sock(sk);
	if (llc->dev)
		dev_put(llc->dev);
	sock_put(sk);
	llc_sk_free(sk);
out:
	return 0;
}