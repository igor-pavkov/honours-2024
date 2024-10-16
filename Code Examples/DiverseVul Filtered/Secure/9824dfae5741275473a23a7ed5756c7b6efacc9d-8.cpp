static struct ipddp_route* __ipddp_find_route(struct ipddp_route *rt)
{
        struct ipddp_route *f;

        for(f = ipddp_route_list; f != NULL; f = f->next)
        {
                if(f->ip == rt->ip &&
		   f->at.s_net == rt->at.s_net &&
		   f->at.s_node == rt->at.s_node)
                        return f;
        }

        return NULL;
}