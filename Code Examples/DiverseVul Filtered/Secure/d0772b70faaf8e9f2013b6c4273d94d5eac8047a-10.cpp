static int __init xfrm6_tunnel_init(void)
{
	if (xfrm_register_type(&xfrm6_tunnel_type, AF_INET6) < 0)
		return -EAGAIN;

	if (xfrm6_tunnel_register(&xfrm6_tunnel_handler, AF_INET6)) {
		xfrm_unregister_type(&xfrm6_tunnel_type, AF_INET6);
		return -EAGAIN;
	}
	if (xfrm6_tunnel_register(&xfrm46_tunnel_handler, AF_INET)) {
		xfrm6_tunnel_deregister(&xfrm6_tunnel_handler, AF_INET6);
		xfrm_unregister_type(&xfrm6_tunnel_type, AF_INET6);
		return -EAGAIN;
	}
	if (xfrm6_tunnel_spi_init() < 0) {
		xfrm6_tunnel_deregister(&xfrm46_tunnel_handler, AF_INET);
		xfrm6_tunnel_deregister(&xfrm6_tunnel_handler, AF_INET6);
		xfrm_unregister_type(&xfrm6_tunnel_type, AF_INET6);
		return -EAGAIN;
	}
	return 0;
}