static void dec_net_namespaces(struct ucounts *ucounts)
{
	dec_ucount(ucounts, UCOUNT_NET_NAMESPACES);
}