static void econet_remove_socket(struct hlist_head *list, struct sock *sk)
{
	write_lock_bh(&econet_lock);
	sk_del_node_init(sk);
	write_unlock_bh(&econet_lock);
}