static void __exit ip6gre_fini(void)
{
	rtnl_link_unregister(&ip6gre_tap_ops);
	rtnl_link_unregister(&ip6gre_link_ops);
	inet6_del_protocol(&ip6gre_protocol, IPPROTO_GRE);
	unregister_pernet_device(&ip6gre_net_ops);
}