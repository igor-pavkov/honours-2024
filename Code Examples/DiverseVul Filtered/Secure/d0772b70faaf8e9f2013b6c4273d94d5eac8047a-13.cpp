static void xfrm6_tunnel_spi_fini(void)
{
	int i;

	for (i = 0; i < XFRM6_TUNNEL_SPI_BYADDR_HSIZE; i++) {
		if (!hlist_empty(&xfrm6_tunnel_spi_byaddr[i]))
			return;
	}
	for (i = 0; i < XFRM6_TUNNEL_SPI_BYSPI_HSIZE; i++) {
		if (!hlist_empty(&xfrm6_tunnel_spi_byspi[i]))
			return;
	}
	kmem_cache_destroy(xfrm6_tunnel_spi_kmem);
	xfrm6_tunnel_spi_kmem = NULL;
}