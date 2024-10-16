static int selinux_socket_setsockopt(struct socket *sock, int level, int optname)
{
	int err;

	err = sock_has_perm(sock->sk, SOCKET__SETOPT);
	if (err)
		return err;

	return selinux_netlbl_socket_setsockopt(sock, level, optname);
}