static int clusterip_tg_check(const struct xt_tgchk_param *par)
{
	struct ipt_clusterip_tgt_info *cipinfo = par->targinfo;
	const struct ipt_entry *e = par->entryinfo;
	struct clusterip_config *config;
	int ret;

	if (cipinfo->hash_mode != CLUSTERIP_HASHMODE_SIP &&
	    cipinfo->hash_mode != CLUSTERIP_HASHMODE_SIP_SPT &&
	    cipinfo->hash_mode != CLUSTERIP_HASHMODE_SIP_SPT_DPT) {
		pr_info("unknown mode %u\n", cipinfo->hash_mode);
		return -EINVAL;

	}
	if (e->ip.dmsk.s_addr != htonl(0xffffffff) ||
	    e->ip.dst.s_addr == 0) {
		pr_info("Please specify destination IP\n");
		return -EINVAL;
	}

	/* FIXME: further sanity checks */

	config = clusterip_config_find_get(e->ip.dst.s_addr, 1);
	if (!config) {
		if (!(cipinfo->flags & CLUSTERIP_FLAG_NEW)) {
			pr_info("no config found for %pI4, need 'new'\n",
				&e->ip.dst.s_addr);
			return -EINVAL;
		} else {
			struct net_device *dev;

			if (e->ip.iniface[0] == '\0') {
				pr_info("Please specify an interface name\n");
				return -EINVAL;
			}

			dev = dev_get_by_name(&init_net, e->ip.iniface);
			if (!dev) {
				pr_info("no such interface %s\n",
					e->ip.iniface);
				return -ENOENT;
			}

			config = clusterip_config_init(cipinfo,
							e->ip.dst.s_addr, dev);
			if (!config) {
				pr_info("cannot allocate config\n");
				dev_put(dev);
				return -ENOMEM;
			}
			dev_mc_add(config->dev, config->clustermac);
		}
	}
	cipinfo->config = config;

	ret = nf_ct_l3proto_try_module_get(par->family);
	if (ret < 0)
		pr_info("cannot load conntrack support for proto=%u\n",
			par->family);
	return ret;
}