static int ip6gre_tunnel_ioctl(struct net_device *dev,
	struct ifreq *ifr, int cmd)
{
	int err = 0;
	struct ip6_tnl_parm2 p;
	struct __ip6_tnl_parm p1;
	struct ip6_tnl *t = netdev_priv(dev);
	struct net *net = t->net;
	struct ip6gre_net *ign = net_generic(net, ip6gre_net_id);

	memset(&p1, 0, sizeof(p1));

	switch (cmd) {
	case SIOCGETTUNNEL:
		if (dev == ign->fb_tunnel_dev) {
			if (copy_from_user(&p, ifr->ifr_ifru.ifru_data, sizeof(p))) {
				err = -EFAULT;
				break;
			}
			ip6gre_tnl_parm_from_user(&p1, &p);
			t = ip6gre_tunnel_locate(net, &p1, 0);
			if (!t)
				t = netdev_priv(dev);
		}
		memset(&p, 0, sizeof(p));
		ip6gre_tnl_parm_to_user(&p, &t->parms);
		if (copy_to_user(ifr->ifr_ifru.ifru_data, &p, sizeof(p)))
			err = -EFAULT;
		break;

	case SIOCADDTUNNEL:
	case SIOCCHGTUNNEL:
		err = -EPERM;
		if (!ns_capable(net->user_ns, CAP_NET_ADMIN))
			goto done;

		err = -EFAULT;
		if (copy_from_user(&p, ifr->ifr_ifru.ifru_data, sizeof(p)))
			goto done;

		err = -EINVAL;
		if ((p.i_flags|p.o_flags)&(GRE_VERSION|GRE_ROUTING))
			goto done;

		if (!(p.i_flags&GRE_KEY))
			p.i_key = 0;
		if (!(p.o_flags&GRE_KEY))
			p.o_key = 0;

		ip6gre_tnl_parm_from_user(&p1, &p);
		t = ip6gre_tunnel_locate(net, &p1, cmd == SIOCADDTUNNEL);

		if (dev != ign->fb_tunnel_dev && cmd == SIOCCHGTUNNEL) {
			if (t) {
				if (t->dev != dev) {
					err = -EEXIST;
					break;
				}
			} else {
				t = netdev_priv(dev);

				ip6gre_tunnel_unlink(ign, t);
				synchronize_net();
				ip6gre_tnl_change(t, &p1, 1);
				ip6gre_tunnel_link(ign, t);
				netdev_state_change(dev);
			}
		}

		if (t) {
			err = 0;

			memset(&p, 0, sizeof(p));
			ip6gre_tnl_parm_to_user(&p, &t->parms);
			if (copy_to_user(ifr->ifr_ifru.ifru_data, &p, sizeof(p)))
				err = -EFAULT;
		} else
			err = (cmd == SIOCADDTUNNEL ? -ENOBUFS : -ENOENT);
		break;

	case SIOCDELTUNNEL:
		err = -EPERM;
		if (!ns_capable(net->user_ns, CAP_NET_ADMIN))
			goto done;

		if (dev == ign->fb_tunnel_dev) {
			err = -EFAULT;
			if (copy_from_user(&p, ifr->ifr_ifru.ifru_data, sizeof(p)))
				goto done;
			err = -ENOENT;
			ip6gre_tnl_parm_from_user(&p1, &p);
			t = ip6gre_tunnel_locate(net, &p1, 0);
			if (!t)
				goto done;
			err = -EPERM;
			if (t == netdev_priv(ign->fb_tunnel_dev))
				goto done;
			dev = t->dev;
		}
		unregister_netdevice(dev);
		err = 0;
		break;

	default:
		err = -EINVAL;
	}

done:
	return err;
}