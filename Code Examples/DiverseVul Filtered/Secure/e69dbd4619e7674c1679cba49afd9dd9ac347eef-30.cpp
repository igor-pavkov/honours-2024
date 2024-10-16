static int raw_hash(struct sock *sk)
{
	write_lock_bh(&raw_lock);
	sk_add_node(sk, &raw_head);
	sock_prot_inuse_add(sock_net(sk), sk->sk_prot, 1);
	write_unlock_bh(&raw_lock);

	return 0;
}