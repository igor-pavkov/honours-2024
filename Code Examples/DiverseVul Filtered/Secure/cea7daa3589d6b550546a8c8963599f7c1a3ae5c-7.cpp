static void keyring_describe(const struct key *keyring, struct seq_file *m)
{
	struct keyring_list *klist;

	if (keyring->description) {
		seq_puts(m, keyring->description);
	}
	else {
		seq_puts(m, "[anon]");
	}

	rcu_read_lock();
	klist = rcu_dereference(keyring->payload.subscriptions);
	if (klist)
		seq_printf(m, ": %u/%u", klist->nkeys, klist->maxkeys);
	else
		seq_puts(m, ": empty");
	rcu_read_unlock();

} /* end keyring_describe() */