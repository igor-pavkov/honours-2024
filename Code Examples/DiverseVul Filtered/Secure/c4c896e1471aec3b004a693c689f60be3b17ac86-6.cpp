static struct sock *sco_get_sock_listen(bdaddr_t *src)
{
	struct sock *sk = NULL, *sk1 = NULL;
	struct hlist_node *node;

	read_lock(&sco_sk_list.lock);

	sk_for_each(sk, node, &sco_sk_list.head) {
		if (sk->sk_state != BT_LISTEN)
			continue;

		/* Exact match. */
		if (!bacmp(&bt_sk(sk)->src, src))
			break;

		/* Closest match */
		if (!bacmp(&bt_sk(sk)->src, BDADDR_ANY))
			sk1 = sk;
	}

	read_unlock(&sco_sk_list.lock);

	return node ? sk : sk1;
}