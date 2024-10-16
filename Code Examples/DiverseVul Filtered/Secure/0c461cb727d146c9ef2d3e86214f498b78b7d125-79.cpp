static int selinux_socket_post_create(struct socket *sock, int family,
				      int type, int protocol, int kern)
{
	const struct task_security_struct *tsec = current_security();
	struct inode_security_struct *isec = inode_security_novalidate(SOCK_INODE(sock));
	struct sk_security_struct *sksec;
	u16 sclass = socket_type_to_security_class(family, type, protocol);
	u32 sid = SECINITSID_KERNEL;
	int err = 0;

	if (!kern) {
		err = socket_sockcreate_sid(tsec, sclass, &sid);
		if (err)
			return err;
	}

	isec->sclass = sclass;
	isec->sid = sid;
	isec->initialized = LABEL_INITIALIZED;

	if (sock->sk) {
		sksec = sock->sk->sk_security;
		sksec->sclass = sclass;
		sksec->sid = sid;
		err = selinux_netlbl_socket_post_create(sock->sk, family);
	}

	return err;
}