static int xfrm6_tunnel_spi_init(void)
{
	int i;

	xfrm6_tunnel_spi = 0;
	xfrm6_tunnel_spi_kmem = kmem_cache_create("xfrm6_tunnel_spi",
						  sizeof(struct xfrm6_tunnel_spi),
						  0, SLAB_HWCACHE_ALIGN,
						  NULL, NULL);
	if (!xfrm6_tunnel_spi_kmem)
		return -ENOMEM;

	for (i = 0; i < XFRM6_TUNNEL_SPI_BYADDR_HSIZE; i++)
		INIT_HLIST_HEAD(&xfrm6_tunnel_spi_byaddr[i]);
	for (i = 0; i < XFRM6_TUNNEL_SPI_BYSPI_HSIZE; i++)
		INIT_HLIST_HEAD(&xfrm6_tunnel_spi_byspi[i]);
	return 0;
}