RZ_API ut64 rz_dyldcache_get_slide(RzDyldCache *cache) {
	rz_return_val_if_fail(cache, 0);
	if (!cache->rebase_infos || !cache->rebase_infos->length) {
		return 0;
	}

	size_t i;
	for (i = 0; i < cache->rebase_infos->length; i++) {
		if (cache->rebase_infos->entries[i].info) {
			return cache->rebase_infos->entries[i].info->slide;
		}
	}

	return 0;
}