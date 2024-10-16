static int nr_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len)
{
	struct sock *sk = sock->sk;
	struct nr_sock *nr = nr_sk(sk);
	struct full_sockaddr_ax25 *addr = (struct full_sockaddr_ax25 *)uaddr;
	struct net_device *dev;
	ax25_uid_assoc *user;
	ax25_address *source;

	lock_sock(sk);
	if (!sock_flag(sk, SOCK_ZAPPED)) {
		release_sock(sk);
		return -EINVAL;
	}
	if (addr_len < sizeof(struct sockaddr_ax25) || addr_len > sizeof(struct full_sockaddr_ax25)) {
		release_sock(sk);
		return -EINVAL;
	}
	if (addr_len < (addr->fsa_ax25.sax25_ndigis * sizeof(ax25_address) + sizeof(struct sockaddr_ax25))) {
		release_sock(sk);
		return -EINVAL;
	}
	if (addr->fsa_ax25.sax25_family != AF_NETROM) {
		release_sock(sk);
		return -EINVAL;
	}
	if ((dev = nr_dev_get(&addr->fsa_ax25.sax25_call)) == NULL) {
		SOCK_DEBUG(sk, "NET/ROM: bind failed: invalid node callsign\n");
		release_sock(sk);
		return -EADDRNOTAVAIL;
	}

	/*
	 * Only the super user can set an arbitrary user callsign.
	 */
	if (addr->fsa_ax25.sax25_ndigis == 1) {
		if (!capable(CAP_NET_BIND_SERVICE)) {
			dev_put(dev);
			release_sock(sk);
			return -EACCES;
		}
		nr->user_addr   = addr->fsa_digipeater[0];
		nr->source_addr = addr->fsa_ax25.sax25_call;
	} else {
		source = &addr->fsa_ax25.sax25_call;

		user = ax25_findbyuid(current_euid());
		if (user) {
			nr->user_addr   = user->call;
			ax25_uid_put(user);
		} else {
			if (ax25_uid_policy && !capable(CAP_NET_BIND_SERVICE)) {
				release_sock(sk);
				dev_put(dev);
				return -EPERM;
			}
			nr->user_addr   = *source;
		}

		nr->source_addr = *source;
	}

	nr->device = dev;
	nr_insert_socket(sk);

	sock_reset_flag(sk, SOCK_ZAPPED);
	dev_put(dev);
	release_sock(sk);
	SOCK_DEBUG(sk, "NET/ROM: socket is bound\n");
	return 0;
}