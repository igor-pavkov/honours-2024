static int ppp_ioctl(struct net_device *dev, struct ifreq *ifr)
{
	hdlc_device *hdlc = dev_to_hdlc(dev);
	struct ppp *ppp;
	int result;

	switch (ifr->ifr_settings.type) {
	case IF_GET_PROTO:
		if (dev_to_hdlc(dev)->proto != &proto)
			return -EINVAL;
		ifr->ifr_settings.type = IF_PROTO_PPP;
		return 0; /* return protocol only, no settable parameters */

	case IF_PROTO_PPP:
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;

		if (dev->flags & IFF_UP)
			return -EBUSY;

		/* no settable parameters */

		result = hdlc->attach(dev, ENCODING_NRZ,PARITY_CRC16_PR1_CCITT);
		if (result)
			return result;

		result = attach_hdlc_protocol(dev, &proto, sizeof(struct ppp));
		if (result)
			return result;

		ppp = get_ppp(dev);
		spin_lock_init(&ppp->lock);
		ppp->req_timeout = 2;
		ppp->cr_retries = 10;
		ppp->term_retries = 2;
		ppp->keepalive_interval = 10;
		ppp->keepalive_timeout = 60;

		dev->hard_header_len = sizeof(struct hdlc_header);
		dev->header_ops = &ppp_header_ops;
		dev->type = ARPHRD_PPP;
		call_netdevice_notifiers(NETDEV_POST_TYPE_CHANGE, dev);
		netif_dormant_on(dev);
		return 0;
	}

	return -EINVAL;
}