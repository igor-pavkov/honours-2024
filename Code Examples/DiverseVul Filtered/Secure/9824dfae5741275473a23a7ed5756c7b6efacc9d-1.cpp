static int ipddp_delete(struct ipddp_route *rt)
{
        struct ipddp_route **r = &ipddp_route_list;
        struct ipddp_route *tmp;

	spin_lock_bh(&ipddp_route_lock);
        while((tmp = *r) != NULL)
        {
                if(tmp->ip == rt->ip &&
		   tmp->at.s_net == rt->at.s_net &&
		   tmp->at.s_node == rt->at.s_node)
                {
                        *r = tmp->next;
			spin_unlock_bh(&ipddp_route_lock);
                        kfree(tmp);
                        return 0;
                }
                r = &tmp->next;
        }

	spin_unlock_bh(&ipddp_route_lock);
        return -ENOENT;
}