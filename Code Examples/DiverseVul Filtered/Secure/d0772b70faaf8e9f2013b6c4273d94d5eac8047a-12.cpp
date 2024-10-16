static void xfrm6_tunnel_destroy(struct xfrm_state *x)
{
	xfrm6_tunnel_free_spi((xfrm_address_t *)&x->props.saddr);
}