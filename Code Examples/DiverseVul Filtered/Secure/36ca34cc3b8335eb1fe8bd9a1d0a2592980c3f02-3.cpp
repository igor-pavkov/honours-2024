static void __exit sit_cleanup(void)
{
	xfrm4_tunnel_deregister(&sit_handler, AF_INET6);

	unregister_pernet_gen_device(sit_net_id, &sit_net_ops);
}