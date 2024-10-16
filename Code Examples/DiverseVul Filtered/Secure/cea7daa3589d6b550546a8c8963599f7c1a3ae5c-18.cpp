static void keyring_unlink_rcu_disposal(struct rcu_head *rcu)
{
	struct keyring_list *klist =
		container_of(rcu, struct keyring_list, rcu);

	key_put(klist->keys[klist->delkey]);
	kfree(klist);

} /* end keyring_unlink_rcu_disposal() */