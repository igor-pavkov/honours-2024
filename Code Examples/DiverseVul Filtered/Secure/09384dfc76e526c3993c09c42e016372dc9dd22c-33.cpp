static int irda_release(struct socket *sock)
{
	struct sock *sk = sock->sk;

	IRDA_DEBUG(2, "%s()\n", __func__);

	if (sk == NULL)
		return 0;

	lock_sock(sk);
	sk->sk_state       = TCP_CLOSE;
	sk->sk_shutdown   |= SEND_SHUTDOWN;
	sk->sk_state_change(sk);

	/* Destroy IrDA socket */
	irda_destroy_socket(irda_sk(sk));

	sock_orphan(sk);
	sock->sk   = NULL;
	release_sock(sk);

	/* Purge queues (see sock_init_data()) */
	skb_queue_purge(&sk->sk_receive_queue);

	/* Destroy networking socket if we are the last reference on it,
	 * i.e. if(sk->sk_refcnt == 0) -> sk_free(sk) */
	sock_put(sk);

	/* Notes on socket locking and deallocation... - Jean II
	 * In theory we should put pairs of sock_hold() / sock_put() to
	 * prevent the socket to be destroyed whenever there is an
	 * outstanding request or outstanding incoming packet or event.
	 *
	 * 1) This may include IAS request, both in connect and getsockopt.
	 * Unfortunately, the situation is a bit more messy than it looks,
	 * because we close iriap and kfree(self) above.
	 *
	 * 2) This may include selective discovery in getsockopt.
	 * Same stuff as above, irlmp registration and self are gone.
	 *
	 * Probably 1 and 2 may not matter, because it's all triggered
	 * by a process and the socket layer already prevent the
	 * socket to go away while a process is holding it, through
	 * sockfd_put() and fput()...
	 *
	 * 3) This may include deferred TSAP closure. In particular,
	 * we may receive a late irda_disconnect_indication()
	 * Fortunately, (tsap_cb *)->close_pend should protect us
	 * from that.
	 *
	 * I did some testing on SMP, and it looks solid. And the socket
	 * memory leak is now gone... - Jean II
	 */

	return 0;
}