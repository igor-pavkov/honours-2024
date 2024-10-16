static void ppp_start(struct net_device *dev)
{
	struct ppp *ppp = get_ppp(dev);
	int i;

	for (i = 0; i < IDX_COUNT; i++) {
		struct proto *proto = &ppp->protos[i];
		proto->dev = dev;
		timer_setup(&proto->timer, ppp_timer, 0);
		proto->state = CLOSED;
	}
	ppp->protos[IDX_LCP].pid = PID_LCP;
	ppp->protos[IDX_IPCP].pid = PID_IPCP;
	ppp->protos[IDX_IPV6CP].pid = PID_IPV6CP;

	ppp_cp_event(dev, PID_LCP, START, 0, 0, 0, NULL);
}