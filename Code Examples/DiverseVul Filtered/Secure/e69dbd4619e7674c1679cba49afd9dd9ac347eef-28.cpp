static int dgram_setsockopt(struct sock *sk, int level, int optname,
			    char __user *optval, unsigned int optlen)
{
	struct dgram_sock *ro = dgram_sk(sk);
	struct net *net = sock_net(sk);
	int val;
	int err = 0;

	if (optlen < sizeof(int))
		return -EINVAL;

	if (get_user(val, (int __user *)optval))
		return -EFAULT;

	lock_sock(sk);

	switch (optname) {
	case WPAN_WANTACK:
		ro->want_ack = !!val;
		break;
	case WPAN_WANTLQI:
		ro->want_lqi = !!val;
		break;
	case WPAN_SECURITY:
		if (!ns_capable(net->user_ns, CAP_NET_ADMIN) &&
		    !ns_capable(net->user_ns, CAP_NET_RAW)) {
			err = -EPERM;
			break;
		}

		switch (val) {
		case WPAN_SECURITY_DEFAULT:
			ro->secen_override = 0;
			break;
		case WPAN_SECURITY_ON:
			ro->secen_override = 1;
			ro->secen = 1;
			break;
		case WPAN_SECURITY_OFF:
			ro->secen_override = 1;
			ro->secen = 0;
			break;
		default:
			err = -EINVAL;
			break;
		}
		break;
	case WPAN_SECURITY_LEVEL:
		if (!ns_capable(net->user_ns, CAP_NET_ADMIN) &&
		    !ns_capable(net->user_ns, CAP_NET_RAW)) {
			err = -EPERM;
			break;
		}

		if (val < WPAN_SECURITY_LEVEL_DEFAULT ||
		    val > IEEE802154_SCF_SECLEVEL_ENC_MIC128) {
			err = -EINVAL;
		} else if (val == WPAN_SECURITY_LEVEL_DEFAULT) {
			ro->seclevel_override = 0;
		} else {
			ro->seclevel_override = 1;
			ro->seclevel = val;
		}
		break;
	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}