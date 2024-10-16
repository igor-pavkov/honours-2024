RZ_API RzDyldCache *rz_dyldcache_new_buf(RzBuffer *buf) {
	RzDyldCache *cache = RZ_NEW0(RzDyldCache);
	if (!cache) {
		return NULL;
	}
	memcpy(cache->magic, "dyldcac", 7);
	cache->buf = rz_buf_ref(buf);
	populate_cache_headers(cache);
	if (!cache->hdr) {
		goto cupertino;
	}
	populate_cache_maps(cache);
	if (!cache->maps) {
		goto cupertino;
	}
	cache->accel = read_cache_accel(cache->buf, cache->hdr, cache->maps);
	cache->bins = create_cache_bins(cache);
	if (!cache->bins) {
		goto cupertino;
	}
	cache->locsym = rz_dyld_locsym_new(cache);
	cache->rebase_infos = get_rebase_infos(cache);
	return cache;
cupertino:
	rz_dyldcache_free(cache);
	return NULL;
}