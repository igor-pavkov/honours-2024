static int ipddp_create(struct ipddp_route *new_rt)
{
        struct ipddp_route *rt = kzalloc(sizeof(*rt), GFP_KERNEL);

        if (rt == NULL)
                return -ENOMEM;

        rt->ip = new_rt->ip;
        rt->at = new_rt->at;
        rt->next = NULL;
        if ((rt->dev = atrtr_get_dev(&rt->at)) == NULL) {
		kfree(rt);
                return -ENETUNREACH;
        }

	spin_lock_bh(&ipddp_route_lock);
	if (__ipddp_find_route(rt)) {
		spin_unlock_bh(&ipddp_route_lock);
		kfree(rt);
		return -EEXIST;
	}

        rt->next = ipddp_route_list;
        ipddp_route_list = rt;

	spin_unlock_bh(&ipddp_route_lock);

        return 0;
}