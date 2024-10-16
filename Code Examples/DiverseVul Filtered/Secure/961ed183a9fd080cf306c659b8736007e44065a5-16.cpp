static void clusterip_config_rcu_free(struct rcu_head *head)
{
	kfree(container_of(head, struct clusterip_config, rcu));
}