static int sco_debugfs_show(struct seq_file *f, void *p)
{
	struct sock *sk;
	struct hlist_node *node;

	read_lock_bh(&sco_sk_list.lock);

	sk_for_each(sk, node, &sco_sk_list.head) {
		seq_printf(f, "%s %s %d\n", batostr(&bt_sk(sk)->src),
				batostr(&bt_sk(sk)->dst), sk->sk_state);
	}

	read_unlock_bh(&sco_sk_list.lock);

	return 0;
}