static int pvc_create(struct net *net, struct socket *sock, int protocol,
		      int kern)
{
	if (net != &init_net)
		return -EAFNOSUPPORT;

	sock->ops = &pvc_proto_ops;
	return vcc_create(net, sock, protocol, PF_ATMPVC);
}