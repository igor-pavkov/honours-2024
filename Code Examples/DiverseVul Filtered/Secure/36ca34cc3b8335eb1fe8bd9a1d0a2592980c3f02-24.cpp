static int sit_init_net(struct net *net)
{
	int err;
	struct sit_net *sitn;

	err = -ENOMEM;
	sitn = kzalloc(sizeof(struct sit_net), GFP_KERNEL);
	if (sitn == NULL)
		goto err_alloc;

	err = net_assign_generic(net, sit_net_id, sitn);
	if (err < 0)
		goto err_assign;

	sitn->tunnels[0] = sitn->tunnels_wc;
	sitn->tunnels[1] = sitn->tunnels_l;
	sitn->tunnels[2] = sitn->tunnels_r;
	sitn->tunnels[3] = sitn->tunnels_r_l;

	sitn->fb_tunnel_dev = alloc_netdev(sizeof(struct ip_tunnel), "sit0",
					   ipip6_tunnel_setup);
	if (!sitn->fb_tunnel_dev) {
		err = -ENOMEM;
		goto err_alloc_dev;
	}

	sitn->fb_tunnel_dev->init = ipip6_fb_tunnel_init;
	dev_net_set(sitn->fb_tunnel_dev, net);

	if ((err = register_netdev(sitn->fb_tunnel_dev)))
		goto err_reg_dev;

	return 0;

err_reg_dev:
	free_netdev(sitn->fb_tunnel_dev);
err_alloc_dev:
	/* nothing */
err_assign:
	kfree(sitn);
err_alloc:
	return err;
}