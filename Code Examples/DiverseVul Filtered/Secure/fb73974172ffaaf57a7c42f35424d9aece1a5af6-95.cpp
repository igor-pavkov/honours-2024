static int selinux_socket_connect(struct socket *sock,
				  struct sockaddr *address, int addrlen)
{
	int err;
	struct sock *sk = sock->sk;

	err = selinux_socket_connect_helper(sock, address, addrlen);
	if (err)
		return err;

	return selinux_netlbl_socket_connect(sk, address);
}