static int __init ip6gre_init(void)
{
	int err;

	pr_info("GRE over IPv6 tunneling driver\n");

	err = register_pernet_device(&ip6gre_net_ops);
	if (err < 0)
		return err;

	err = inet6_add_protocol(&ip6gre_protocol, IPPROTO_GRE);
	if (err < 0) {
		pr_info("%s: can't add protocol\n", __func__);
		goto add_proto_failed;
	}

	err = rtnl_link_register(&ip6gre_link_ops);
	if (err < 0)
		goto rtnl_link_failed;

	err = rtnl_link_register(&ip6gre_tap_ops);
	if (err < 0)
		goto tap_ops_failed;

out:
	return err;

tap_ops_failed:
	rtnl_link_unregister(&ip6gre_link_ops);
rtnl_link_failed:
	inet6_del_protocol(&ip6gre_protocol, IPPROTO_GRE);
add_proto_failed:
	unregister_pernet_device(&ip6gre_net_ops);
	goto out;
}