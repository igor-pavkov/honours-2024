static int raw_init(struct sock *sk)
{
	struct raw_sock *ro = raw_sk(sk);

	ro->bound            = 0;
	ro->ifindex          = 0;

	/* set default filter to single entry dfilter */
	ro->dfilter.can_id   = 0;
	ro->dfilter.can_mask = MASK_ALL;
	ro->filter           = &ro->dfilter;
	ro->count            = 1;

	/* set default loopback behaviour */
	ro->loopback         = 1;
	ro->recv_own_msgs    = 0;

	/* set notifier */
	ro->notifier.notifier_call = raw_notifier;

	register_netdevice_notifier(&ro->notifier);

	return 0;
}