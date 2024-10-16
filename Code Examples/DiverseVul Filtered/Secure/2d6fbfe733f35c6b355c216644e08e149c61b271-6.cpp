static int setsockopt(struct socket *sock,
			int lvl, int opt, char __user *ov, unsigned int ol)
{
	struct sock *sk = sock->sk;
	struct caifsock *cf_sk = container_of(sk, struct caifsock, sk);
	int linksel;

	if (cf_sk->sk.sk_socket->state != SS_UNCONNECTED)
		return -ENOPROTOOPT;

	switch (opt) {
	case CAIFSO_LINK_SELECT:
		if (ol < sizeof(int))
			return -EINVAL;
		if (lvl != SOL_CAIF)
			goto bad_sol;
		if (copy_from_user(&linksel, ov, sizeof(int)))
			return -EINVAL;
		lock_sock(&(cf_sk->sk));
		cf_sk->conn_req.link_selector = linksel;
		release_sock(&cf_sk->sk);
		return 0;

	case CAIFSO_REQ_PARAM:
		if (lvl != SOL_CAIF)
			goto bad_sol;
		if (cf_sk->sk.sk_protocol != CAIFPROTO_UTIL)
			return -ENOPROTOOPT;
		lock_sock(&(cf_sk->sk));
		if (ol > sizeof(cf_sk->conn_req.param.data) ||
			copy_from_user(&cf_sk->conn_req.param.data, ov, ol)) {
			release_sock(&cf_sk->sk);
			return -EINVAL;
		}
		cf_sk->conn_req.param.size = ol;
		release_sock(&cf_sk->sk);
		return 0;

	default:
		return -ENOPROTOOPT;
	}

	return 0;
bad_sol:
	return -ENOPROTOOPT;

}