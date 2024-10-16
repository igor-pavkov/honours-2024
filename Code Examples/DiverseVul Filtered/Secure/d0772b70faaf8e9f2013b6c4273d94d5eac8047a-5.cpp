__be32 xfrm6_tunnel_spi_lookup(xfrm_address_t *saddr)
{
	struct xfrm6_tunnel_spi *x6spi;
	u32 spi;

	read_lock_bh(&xfrm6_tunnel_spi_lock);
	x6spi = __xfrm6_tunnel_spi_lookup(saddr);
	spi = x6spi ? x6spi->spi : 0;
	read_unlock_bh(&xfrm6_tunnel_spi_lock);
	return htonl(spi);
}