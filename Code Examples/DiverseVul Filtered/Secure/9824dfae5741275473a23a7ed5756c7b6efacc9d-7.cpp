static int ipddp_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
        struct ipddp_route __user *rt = ifr->ifr_data;
        struct ipddp_route rcp, rcp2, *rp;

        if(!capable(CAP_NET_ADMIN))
                return -EPERM;

	if(copy_from_user(&rcp, rt, sizeof(rcp)))
		return -EFAULT;

        switch(cmd)
        {
		case SIOCADDIPDDPRT:
                        return ipddp_create(&rcp);

                case SIOCFINDIPDDPRT:
			spin_lock_bh(&ipddp_route_lock);
			rp = __ipddp_find_route(&rcp);
			if (rp) {
				memset(&rcp2, 0, sizeof(rcp2));
				rcp2.ip    = rp->ip;
				rcp2.at    = rp->at;
				rcp2.flags = rp->flags;
			}
			spin_unlock_bh(&ipddp_route_lock);

			if (rp) {
				if (copy_to_user(rt, &rcp2,
						 sizeof(struct ipddp_route)))
					return -EFAULT;
				return 0;
			} else
				return -ENOENT;

                case SIOCDELIPDDPRT:
                        return ipddp_delete(&rcp);

                default:
                        return -EINVAL;
        }
}