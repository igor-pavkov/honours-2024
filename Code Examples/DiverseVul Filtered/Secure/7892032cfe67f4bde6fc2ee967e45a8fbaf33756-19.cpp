static struct ip6_tnl *ip6gre_tunnel_lookup(struct net_device *dev,
		const struct in6_addr *remote, const struct in6_addr *local,
		__be32 key, __be16 gre_proto)
{
	struct net *net = dev_net(dev);
	int link = dev->ifindex;
	unsigned int h0 = HASH_ADDR(remote);
	unsigned int h1 = HASH_KEY(key);
	struct ip6_tnl *t, *cand = NULL;
	struct ip6gre_net *ign = net_generic(net, ip6gre_net_id);
	int dev_type = (gre_proto == htons(ETH_P_TEB)) ?
		       ARPHRD_ETHER : ARPHRD_IP6GRE;
	int score, cand_score = 4;

	for_each_ip_tunnel_rcu(t, ign->tunnels_r_l[h0 ^ h1]) {
		if (!ipv6_addr_equal(local, &t->parms.laddr) ||
		    !ipv6_addr_equal(remote, &t->parms.raddr) ||
		    key != t->parms.i_key ||
		    !(t->dev->flags & IFF_UP))
			continue;

		if (t->dev->type != ARPHRD_IP6GRE &&
		    t->dev->type != dev_type)
			continue;

		score = 0;
		if (t->parms.link != link)
			score |= 1;
		if (t->dev->type != dev_type)
			score |= 2;
		if (score == 0)
			return t;

		if (score < cand_score) {
			cand = t;
			cand_score = score;
		}
	}

	for_each_ip_tunnel_rcu(t, ign->tunnels_r[h0 ^ h1]) {
		if (!ipv6_addr_equal(remote, &t->parms.raddr) ||
		    key != t->parms.i_key ||
		    !(t->dev->flags & IFF_UP))
			continue;

		if (t->dev->type != ARPHRD_IP6GRE &&
		    t->dev->type != dev_type)
			continue;

		score = 0;
		if (t->parms.link != link)
			score |= 1;
		if (t->dev->type != dev_type)
			score |= 2;
		if (score == 0)
			return t;

		if (score < cand_score) {
			cand = t;
			cand_score = score;
		}
	}

	for_each_ip_tunnel_rcu(t, ign->tunnels_l[h1]) {
		if ((!ipv6_addr_equal(local, &t->parms.laddr) &&
			  (!ipv6_addr_equal(local, &t->parms.raddr) ||
				 !ipv6_addr_is_multicast(local))) ||
		    key != t->parms.i_key ||
		    !(t->dev->flags & IFF_UP))
			continue;

		if (t->dev->type != ARPHRD_IP6GRE &&
		    t->dev->type != dev_type)
			continue;

		score = 0;
		if (t->parms.link != link)
			score |= 1;
		if (t->dev->type != dev_type)
			score |= 2;
		if (score == 0)
			return t;

		if (score < cand_score) {
			cand = t;
			cand_score = score;
		}
	}

	for_each_ip_tunnel_rcu(t, ign->tunnels_wc[h1]) {
		if (t->parms.i_key != key ||
		    !(t->dev->flags & IFF_UP))
			continue;

		if (t->dev->type != ARPHRD_IP6GRE &&
		    t->dev->type != dev_type)
			continue;

		score = 0;
		if (t->parms.link != link)
			score |= 1;
		if (t->dev->type != dev_type)
			score |= 2;
		if (score == 0)
			return t;

		if (score < cand_score) {
			cand = t;
			cand_score = score;
		}
	}

	if (cand)
		return cand;

	dev = ign->fb_tunnel_dev;
	if (dev->flags & IFF_UP)
		return netdev_priv(dev);

	return NULL;
}