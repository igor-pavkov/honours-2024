__be32 xfrm6_tunnel_alloc_spi(xfrm_address_t *saddr)
{
	struct xfrm6_tunnel_spi *x6spi;
	u32 spi;

	write_lock_bh(&xfrm6_tunnel_spi_lock);
	x6spi = __xfrm6_tunnel_spi_lookup(saddr);
	if (x6spi) {
		atomic_inc(&x6spi->refcnt);
		spi = x6spi->spi;
	} else
		spi = __xfrm6_tunnel_alloc_spi(saddr);
	write_unlock_bh(&xfrm6_tunnel_spi_lock);

	return htonl(spi);
}