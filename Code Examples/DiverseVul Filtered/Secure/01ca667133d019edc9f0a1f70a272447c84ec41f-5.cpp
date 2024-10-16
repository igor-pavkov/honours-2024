static bool fm10k_can_reuse_rx_page(struct fm10k_rx_buffer *rx_buffer,
				    struct page *page,
				    unsigned int __maybe_unused truesize)
{
	/* avoid re-using remote pages */
	if (unlikely(fm10k_page_is_reserved(page)))
		return false;

#if (PAGE_SIZE < 8192)
	/* if we are only owner of page we can reuse it */
	if (unlikely(page_count(page) != 1))
		return false;

	/* flip page offset to other buffer */
	rx_buffer->page_offset ^= FM10K_RX_BUFSZ;
#else
	/* move offset up to the next cache line */
	rx_buffer->page_offset += truesize;

	if (rx_buffer->page_offset > (PAGE_SIZE - FM10K_RX_BUFSZ))
		return false;
#endif

	/* Even if we own the page, we are not allowed to use atomic_set()
	 * This would break get_page_unless_zero() users.
	 */
	page_ref_inc(page);

	return true;
}