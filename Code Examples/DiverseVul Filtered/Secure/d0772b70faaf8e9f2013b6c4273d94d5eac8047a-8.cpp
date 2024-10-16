void xfrm6_tunnel_free_spi(xfrm_address_t *saddr)
{
	struct xfrm6_tunnel_spi *x6spi;
	struct hlist_node *pos, *n;

	write_lock_bh(&xfrm6_tunnel_spi_lock);

	hlist_for_each_entry_safe(x6spi, pos, n,
				  &xfrm6_tunnel_spi_byaddr[xfrm6_tunnel_spi_hash_byaddr(saddr)],
				  list_byaddr)
	{
		if (memcmp(&x6spi->addr, saddr, sizeof(x6spi->addr)) == 0) {
			if (atomic_dec_and_test(&x6spi->refcnt)) {
				hlist_del(&x6spi->list_byaddr);
				hlist_del(&x6spi->list_byspi);
				kmem_cache_free(xfrm6_tunnel_spi_kmem, x6spi);
				break;
			}
		}
	}
	write_unlock_bh(&xfrm6_tunnel_spi_lock);
}