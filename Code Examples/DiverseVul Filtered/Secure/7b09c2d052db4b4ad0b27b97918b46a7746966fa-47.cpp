void fib6_info_destroy_rcu(struct rcu_head *head)
{
	struct fib6_info *f6i = container_of(head, struct fib6_info, rcu);

	WARN_ON(f6i->fib6_node);

	if (f6i->nh)
		nexthop_put(f6i->nh);
	else
		fib6_nh_release(f6i->fib6_nh);

	ip_fib_metrics_put(f6i->fib6_metrics);
	kfree(f6i);
}