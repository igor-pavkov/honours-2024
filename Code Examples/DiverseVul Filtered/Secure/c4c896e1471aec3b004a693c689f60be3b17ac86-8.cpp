static struct sock *__sco_get_sock_by_addr(bdaddr_t *ba)
{
	struct sock *sk;
	struct hlist_node *node;

	sk_for_each(sk, node, &sco_sk_list.head)
		if (!bacmp(&bt_sk(sk)->src, ba))
			goto found;
	sk = NULL;
found:
	return sk;
}