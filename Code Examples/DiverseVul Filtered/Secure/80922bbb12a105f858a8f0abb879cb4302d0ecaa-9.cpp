static void econet_insert_socket(struct hlist_head *list, struct sock *sk)
{
	write_lock_bh(&econet_lock);
	sk_add_node(sk, list);
	write_unlock_bh(&econet_lock);
}