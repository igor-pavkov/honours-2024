RZ_API void rz_dyldcache_free(RzDyldCache *cache) {
	if (!cache) {
		return;
	}

	rz_list_free(cache->bins);
	cache->bins = NULL;
	rz_buf_free(cache->buf);
	cache->buf = NULL;
	if (cache->rebase_infos) {
		int i;
		for (i = 0; i < cache->rebase_infos->length; i++) {
			rebase_info_free(cache->rebase_infos->entries[i].info);
			cache->rebase_infos->entries[i].info = NULL;
		}
		RZ_FREE(cache->rebase_infos->entries);
		RZ_FREE(cache->rebase_infos);
	}
	free(cache->hdr);
	free(cache->maps);
	free(cache->maps_index);
	free(cache->hdr_offset);
	free(cache->accel);
	free(cache->locsym);
	free(cache->oi);
	free(cache);
}