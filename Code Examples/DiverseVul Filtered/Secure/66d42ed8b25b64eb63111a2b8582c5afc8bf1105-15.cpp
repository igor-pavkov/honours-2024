static inline struct proto* get_proto(struct net_device *dev, u16 pid)
{
	struct ppp *ppp = get_ppp(dev);

	switch (pid) {
	case PID_LCP:
		return &ppp->protos[IDX_LCP];
	case PID_IPCP:
		return &ppp->protos[IDX_IPCP];
	case PID_IPV6CP:
		return &ppp->protos[IDX_IPV6CP];
	default:
		return NULL;
	}
}