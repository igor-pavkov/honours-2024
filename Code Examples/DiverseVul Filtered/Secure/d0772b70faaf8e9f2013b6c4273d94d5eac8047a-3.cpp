static struct xfrm6_tunnel_spi *__xfrm6_tunnel_spi_lookup(xfrm_address_t *saddr)
{
	struct xfrm6_tunnel_spi *x6spi;
	struct hlist_node *pos;

	hlist_for_each_entry(x6spi, pos,
			     &xfrm6_tunnel_spi_byaddr[xfrm6_tunnel_spi_hash_byaddr(saddr)],
			     list_byaddr) {
		if (memcmp(&x6spi->addr, saddr, sizeof(x6spi->addr)) == 0)
			return x6spi;
	}

	return NULL;
}