static void br_multicast_free_pg(struct rcu_head *head)
{
	struct net_bridge_port_group *p =
		container_of(head, struct net_bridge_port_group, rcu);

	kfree(p);
}