void sk_free_unlock_clone(struct sock *sk)
{
	/* It is still raw copy of parent, so invalidate
	 * destructor and make plain sk_free() */
	sk->sk_destruct = NULL;
	bh_unlock_sock(sk);
	sk_free(sk);
}