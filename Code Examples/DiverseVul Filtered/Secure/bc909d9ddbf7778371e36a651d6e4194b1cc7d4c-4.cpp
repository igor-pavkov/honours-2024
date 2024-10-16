void sock_unregister(int family)
{
	BUG_ON(family < 0 || family >= NPROTO);

	spin_lock(&net_family_lock);
	rcu_assign_pointer(net_families[family], NULL);
	spin_unlock(&net_family_lock);

	synchronize_rcu();

	printk(KERN_INFO "NET: Unregistered protocol family %d\n", family);
}