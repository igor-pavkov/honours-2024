static void keyring_link_rcu_disposal(struct rcu_head *rcu)
{
	struct keyring_list *klist =
		container_of(rcu, struct keyring_list, rcu);

	kfree(klist);

} /* end keyring_link_rcu_disposal() */