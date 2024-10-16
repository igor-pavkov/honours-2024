static void ipip6_tunnel_link(struct sit_net *sitn, struct ip_tunnel *t)
{
	struct ip_tunnel **tp = ipip6_bucket(sitn, t);

	t->next = *tp;
	write_lock_bh(&ipip6_lock);
	*tp = t;
	write_unlock_bh(&ipip6_lock);
}