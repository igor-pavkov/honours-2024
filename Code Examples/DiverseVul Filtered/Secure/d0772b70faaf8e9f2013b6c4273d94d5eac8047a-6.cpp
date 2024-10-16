static u32 __xfrm6_tunnel_alloc_spi(xfrm_address_t *saddr)
{
	u32 spi;
	struct xfrm6_tunnel_spi *x6spi;
	struct hlist_node *pos;
	unsigned index;

	if (xfrm6_tunnel_spi < XFRM6_TUNNEL_SPI_MIN ||
	    xfrm6_tunnel_spi >= XFRM6_TUNNEL_SPI_MAX)
		xfrm6_tunnel_spi = XFRM6_TUNNEL_SPI_MIN;
	else
		xfrm6_tunnel_spi++;

	for (spi = xfrm6_tunnel_spi; spi <= XFRM6_TUNNEL_SPI_MAX; spi++) {
		index = xfrm6_tunnel_spi_hash_byspi(spi);
		hlist_for_each_entry(x6spi, pos,
				     &xfrm6_tunnel_spi_byspi[index],
				     list_byspi) {
			if (x6spi->spi == spi)
				goto try_next_1;
		}
		xfrm6_tunnel_spi = spi;
		goto alloc_spi;
try_next_1:;
	}
	for (spi = XFRM6_TUNNEL_SPI_MIN; spi < xfrm6_tunnel_spi; spi++) {
		index = xfrm6_tunnel_spi_hash_byspi(spi);
		hlist_for_each_entry(x6spi, pos,
				     &xfrm6_tunnel_spi_byspi[index],
				     list_byspi) {
			if (x6spi->spi == spi)
				goto try_next_2;
		}
		xfrm6_tunnel_spi = spi;
		goto alloc_spi;
try_next_2:;
	}
	spi = 0;
	goto out;
alloc_spi:
	x6spi = kmem_cache_alloc(xfrm6_tunnel_spi_kmem, GFP_ATOMIC);
	if (!x6spi)
		goto out;

	memcpy(&x6spi->addr, saddr, sizeof(x6spi->addr));
	x6spi->spi = spi;
	atomic_set(&x6spi->refcnt, 1);

	hlist_add_head(&x6spi->list_byspi, &xfrm6_tunnel_spi_byspi[index]);

	index = xfrm6_tunnel_spi_hash_byaddr(saddr);
	hlist_add_head(&x6spi->list_byaddr, &xfrm6_tunnel_spi_byaddr[index]);
out:
	return spi;
}