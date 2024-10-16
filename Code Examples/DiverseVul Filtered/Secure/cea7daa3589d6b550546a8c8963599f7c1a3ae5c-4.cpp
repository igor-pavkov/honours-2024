static void keyring_clear_rcu_disposal(struct rcu_head *rcu)
{
	struct keyring_list *klist;
	int loop;

	klist = container_of(rcu, struct keyring_list, rcu);

	for (loop = klist->nkeys - 1; loop >= 0; loop--)
		key_put(klist->keys[loop]);

	kfree(klist);

} /* end keyring_clear_rcu_disposal() */