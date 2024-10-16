static int sco_sock_bind(struct socket *sock, struct sockaddr *addr, int addr_len)
{
	struct sockaddr_sco *sa = (struct sockaddr_sco *) addr;
	struct sock *sk = sock->sk;
	bdaddr_t *src = &sa->sco_bdaddr;
	int err = 0;

	BT_DBG("sk %p %s", sk, batostr(&sa->sco_bdaddr));

	if (!addr || addr->sa_family != AF_BLUETOOTH)
		return -EINVAL;

	lock_sock(sk);

	if (sk->sk_state != BT_OPEN) {
		err = -EBADFD;
		goto done;
	}

	write_lock_bh(&sco_sk_list.lock);

	if (bacmp(src, BDADDR_ANY) && __sco_get_sock_by_addr(src)) {
		err = -EADDRINUSE;
	} else {
		/* Save source address */
		bacpy(&bt_sk(sk)->src, &sa->sco_bdaddr);
		sk->sk_state = BT_BOUND;
	}

	write_unlock_bh(&sco_sk_list.lock);

done:
	release_sock(sk);
	return err;
}