static cache_hdr_t *read_cache_header(RzBuffer *cache_buf, ut64 offset) {
	if (!cache_buf) {
		return NULL;
	}

	cache_hdr_t *hdr = RZ_NEW0(cache_hdr_t);
	if (!hdr) {
		return NULL;
	}

	ut64 size = sizeof(cache_hdr_t);
	if (rz_buf_fread_at(cache_buf, offset, (ut8 *)hdr, "16c4i7l16clii4l", 1) != size) {
		free(hdr);
		return NULL;
	}
	if (!rz_dyldcache_check_magic(hdr->magic)) {
		free(hdr);
		return NULL;
	}

	if (!hdr->imagesCount && !hdr->imagesOffset) {
		if (!rz_buf_read_le32_at(cache_buf, 0x1c0 + offset, &hdr->imagesOffset) || !rz_buf_read_le32_at(cache_buf, 0x1c4 + offset, &hdr->imagesCount)) {
			free(hdr);
			return NULL;
		}
	}
	return hdr;
}