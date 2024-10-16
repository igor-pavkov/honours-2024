static void rfcomm_sock_destruct(struct sock *sk)
{
	struct rfcomm_dlc *d = rfcomm_pi(sk)->dlc;

	BT_DBG("sk %p dlc %p", sk, d);

	skb_queue_purge(&sk->sk_receive_queue);
	skb_queue_purge(&sk->sk_write_queue);

	rfcomm_dlc_lock(d);
	rfcomm_pi(sk)->dlc = NULL;

	/* Detach DLC if it's owned by this socket */
	if (d->owner == sk)
		d->owner = NULL;
	rfcomm_dlc_unlock(d);

	rfcomm_dlc_put(d);
}