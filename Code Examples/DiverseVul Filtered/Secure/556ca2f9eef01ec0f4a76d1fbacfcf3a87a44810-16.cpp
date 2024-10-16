static void populate_cache_maps(RzDyldCache *cache) {
	rz_return_if_fail(cache && cache->buf);

	ut32 i;
	ut32 n_maps = 0;
	ut64 max_count = 0;
	for (i = 0; i < cache->n_hdr; i++) {
		cache_hdr_t *hdr = &cache->hdr[i];
		if (!hdr->mappingCount || !hdr->mappingOffset) {
			continue;
		}
		max_count = RZ_MAX(hdr->mappingCount, max_count);
		n_maps += hdr->mappingCount;
	}

	if (n_maps < 1 || n_maps < max_count /* overflow */) {
		cache->maps = NULL;
		cache->n_maps = 0;
		return;
	}

	cache->maps_index = RZ_NEWS0(ut32, cache->n_hdr);
	if (!cache->maps_index) {
		return;
	}
	cache_map_t *maps = RZ_NEWS0(cache_map_t, n_maps);

	ut32 next_map = 0;
	ut32 last_idx = UT32_MAX;
	ut64 max_address = 0;
	for (i = 0; i < cache->n_hdr; i++) {
		cache_hdr_t *hdr = &cache->hdr[i];
		cache->maps_index[i] = next_map;

		if (!hdr->mappingCount || !hdr->mappingOffset) {
			continue;
		}
		ut64 size = sizeof(cache_map_t) * hdr->mappingCount;
		if (rz_buf_fread_at(cache->buf, hdr->mappingOffset, (ut8 *)&maps[next_map], "3l2i", hdr->mappingCount) != size) {
			continue;
		}
		ut32 j;
		ut64 hdr_offset = cache->hdr_offset[i];
		for (j = 0; j < hdr->mappingCount; j++) {
			cache_map_t *map = &maps[next_map + j];
			map->fileOffset += hdr_offset;
			if (map->address > max_address) {
				last_idx = i;
				max_address = map->address;
			}
		}
		next_map += hdr->mappingCount;
	}

	cache->maps = maps;
	cache->n_maps = next_map;
	if (last_idx == UT32_MAX) {
		cache->symbols_off_base = 0;
	} else {
		cache->symbols_off_base = cache->hdr_offset[last_idx];
	}
}