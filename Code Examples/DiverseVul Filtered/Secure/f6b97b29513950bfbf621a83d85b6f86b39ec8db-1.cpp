static void *nr_info_start(struct seq_file *seq, loff_t *pos)
{
	struct sock *s;
	struct hlist_node *node;
	int i = 1;

	spin_lock_bh(&nr_list_lock);
	if (*pos == 0)
		return SEQ_START_TOKEN;

	sk_for_each(s, node, &nr_list) {
		if (i == *pos)
			return s;
		++i;
	}
	return NULL;
}