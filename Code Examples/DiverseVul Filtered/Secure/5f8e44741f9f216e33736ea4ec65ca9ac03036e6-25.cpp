static int rtnl_newlink(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	struct net *net = sock_net(skb->sk);
	const struct rtnl_link_ops *ops;
	const struct rtnl_link_ops *m_ops = NULL;
	struct net_device *dev;
	struct net_device *master_dev = NULL;
	struct ifinfomsg *ifm;
	char kind[MODULE_NAME_LEN];
	char ifname[IFNAMSIZ];
	struct nlattr *tb[IFLA_MAX+1];
	struct nlattr *linkinfo[IFLA_INFO_MAX+1];
	unsigned char name_assign_type = NET_NAME_USER;
	int err;

#ifdef CONFIG_MODULES
replay:
#endif
	err = nlmsg_parse(nlh, sizeof(*ifm), tb, IFLA_MAX, ifla_policy);
	if (err < 0)
		return err;

	if (tb[IFLA_IFNAME])
		nla_strlcpy(ifname, tb[IFLA_IFNAME], IFNAMSIZ);
	else
		ifname[0] = '\0';

	ifm = nlmsg_data(nlh);
	if (ifm->ifi_index > 0)
		dev = __dev_get_by_index(net, ifm->ifi_index);
	else {
		if (ifname[0])
			dev = __dev_get_by_name(net, ifname);
		else
			dev = NULL;
	}

	if (dev) {
		master_dev = netdev_master_upper_dev_get(dev);
		if (master_dev)
			m_ops = master_dev->rtnl_link_ops;
	}

	err = validate_linkmsg(dev, tb);
	if (err < 0)
		return err;

	if (tb[IFLA_LINKINFO]) {
		err = nla_parse_nested(linkinfo, IFLA_INFO_MAX,
				       tb[IFLA_LINKINFO], ifla_info_policy);
		if (err < 0)
			return err;
	} else
		memset(linkinfo, 0, sizeof(linkinfo));

	if (linkinfo[IFLA_INFO_KIND]) {
		nla_strlcpy(kind, linkinfo[IFLA_INFO_KIND], sizeof(kind));
		ops = rtnl_link_ops_get(kind);
	} else {
		kind[0] = '\0';
		ops = NULL;
	}

	if (1) {
		struct nlattr *attr[ops ? ops->maxtype + 1 : 1];
		struct nlattr *slave_attr[m_ops ? m_ops->slave_maxtype + 1 : 1];
		struct nlattr **data = NULL;
		struct nlattr **slave_data = NULL;
		struct net *dest_net, *link_net = NULL;

		if (ops) {
			if (ops->maxtype && linkinfo[IFLA_INFO_DATA]) {
				err = nla_parse_nested(attr, ops->maxtype,
						       linkinfo[IFLA_INFO_DATA],
						       ops->policy);
				if (err < 0)
					return err;
				data = attr;
			}
			if (ops->validate) {
				err = ops->validate(tb, data);
				if (err < 0)
					return err;
			}
		}

		if (m_ops) {
			if (m_ops->slave_maxtype &&
			    linkinfo[IFLA_INFO_SLAVE_DATA]) {
				err = nla_parse_nested(slave_attr,
						       m_ops->slave_maxtype,
						       linkinfo[IFLA_INFO_SLAVE_DATA],
						       m_ops->slave_policy);
				if (err < 0)
					return err;
				slave_data = slave_attr;
			}
			if (m_ops->slave_validate) {
				err = m_ops->slave_validate(tb, slave_data);
				if (err < 0)
					return err;
			}
		}

		if (dev) {
			int status = 0;

			if (nlh->nlmsg_flags & NLM_F_EXCL)
				return -EEXIST;
			if (nlh->nlmsg_flags & NLM_F_REPLACE)
				return -EOPNOTSUPP;

			if (linkinfo[IFLA_INFO_DATA]) {
				if (!ops || ops != dev->rtnl_link_ops ||
				    !ops->changelink)
					return -EOPNOTSUPP;

				err = ops->changelink(dev, tb, data);
				if (err < 0)
					return err;
				status |= DO_SETLINK_NOTIFY;
			}

			if (linkinfo[IFLA_INFO_SLAVE_DATA]) {
				if (!m_ops || !m_ops->slave_changelink)
					return -EOPNOTSUPP;

				err = m_ops->slave_changelink(master_dev, dev,
							      tb, slave_data);
				if (err < 0)
					return err;
				status |= DO_SETLINK_NOTIFY;
			}

			return do_setlink(skb, dev, ifm, tb, ifname, status);
		}

		if (!(nlh->nlmsg_flags & NLM_F_CREATE)) {
			if (ifm->ifi_index == 0 && tb[IFLA_GROUP])
				return rtnl_group_changelink(skb, net,
						nla_get_u32(tb[IFLA_GROUP]),
						ifm, tb);
			return -ENODEV;
		}

		if (tb[IFLA_MAP] || tb[IFLA_MASTER] || tb[IFLA_PROTINFO])
			return -EOPNOTSUPP;

		if (!ops) {
#ifdef CONFIG_MODULES
			if (kind[0]) {
				__rtnl_unlock();
				request_module("rtnl-link-%s", kind);
				rtnl_lock();
				ops = rtnl_link_ops_get(kind);
				if (ops)
					goto replay;
			}
#endif
			return -EOPNOTSUPP;
		}

		if (!ops->setup)
			return -EOPNOTSUPP;

		if (!ifname[0]) {
			snprintf(ifname, IFNAMSIZ, "%s%%d", ops->kind);
			name_assign_type = NET_NAME_ENUM;
		}

		dest_net = rtnl_link_get_net(net, tb);
		if (IS_ERR(dest_net))
			return PTR_ERR(dest_net);

		err = -EPERM;
		if (!netlink_ns_capable(skb, dest_net->user_ns, CAP_NET_ADMIN))
			goto out;

		if (tb[IFLA_LINK_NETNSID]) {
			int id = nla_get_s32(tb[IFLA_LINK_NETNSID]);

			link_net = get_net_ns_by_id(dest_net, id);
			if (!link_net) {
				err =  -EINVAL;
				goto out;
			}
			err = -EPERM;
			if (!netlink_ns_capable(skb, link_net->user_ns, CAP_NET_ADMIN))
				goto out;
		}

		dev = rtnl_create_link(link_net ? : dest_net, ifname,
				       name_assign_type, ops, tb);
		if (IS_ERR(dev)) {
			err = PTR_ERR(dev);
			goto out;
		}

		dev->ifindex = ifm->ifi_index;

		if (ops->newlink) {
			err = ops->newlink(link_net ? : net, dev, tb, data);
			/* Drivers should call free_netdev() in ->destructor
			 * and unregister it on failure after registration
			 * so that device could be finally freed in rtnl_unlock.
			 */
			if (err < 0) {
				/* If device is not registered at all, free it now */
				if (dev->reg_state == NETREG_UNINITIALIZED)
					free_netdev(dev);
				goto out;
			}
		} else {
			err = register_netdevice(dev);
			if (err < 0) {
				free_netdev(dev);
				goto out;
			}
		}
		err = rtnl_configure_link(dev, ifm);
		if (err < 0)
			goto out_unregister;
		if (link_net) {
			err = dev_change_net_namespace(dev, dest_net, ifname);
			if (err < 0)
				goto out_unregister;
		}
out:
		if (link_net)
			put_net(link_net);
		put_net(dest_net);
		return err;
out_unregister:
		if (ops->newlink) {
			LIST_HEAD(list_kill);

			ops->dellink(dev, &list_kill);
			unregister_netdevice_many(&list_kill);
		} else {
			unregister_netdevice(dev);
		}
		goto out;
	}
}