static RzDyldLocSym *rz_dyld_locsym_new(RzDyldCache *cache) {
	rz_return_val_if_fail(cache && cache->buf, NULL);

	ut32 i;
	for (i = 0; i < cache->n_hdr; i++) {
		cache_hdr_t *hdr = &cache->hdr[i];
		if (!hdr || !hdr->localSymbolsSize || !hdr->localSymbolsOffset) {
			continue;
		}

		cache_locsym_info_t *info = NULL;
		void *entries = NULL;

		ut64 info_size = sizeof(cache_locsym_info_t);
		info = RZ_NEW0(cache_locsym_info_t);
		if (!info) {
			goto beach;
		}
		if (rz_buf_fread_at(cache->buf, hdr->localSymbolsOffset, (ut8 *)info, "6i", 1) != info_size) {
			RZ_LOG_ERROR("Cannot read cache_locsym_info_t from header\n");
			goto beach;
		}
		if (info->entriesCount != cache->hdr->imagesCount) {
			RZ_LOG_ERROR("The number of entries count differs from cache header image count\n");
			goto beach;
		}

		bool has_large_entries = cache->n_hdr > 1;
		if (has_large_entries) {
			ut64 entries_size = sizeof(cache_locsym_entry_large_t) * info->entriesCount;
			cache_locsym_entry_large_t *large_entries = RZ_NEWS0(cache_locsym_entry_large_t, info->entriesCount);
			if (!large_entries) {
				goto beach;
			}
			if (rz_buf_fread_at(cache->buf, hdr->localSymbolsOffset + info->entriesOffset, (ut8 *)large_entries, "lii",
				    info->entriesCount) != entries_size) {
				RZ_LOG_ERROR("Cannot read cache_locsym_entry_large_t\n");
				goto beach;
			}
			entries = large_entries;
		} else {
			ut64 entries_size = sizeof(cache_locsym_entry_t) * info->entriesCount;
			cache_locsym_entry_t *regular_entries = RZ_NEWS0(cache_locsym_entry_t, info->entriesCount);
			if (!regular_entries) {
				goto beach;
			}
			if (rz_buf_fread_at(cache->buf, hdr->localSymbolsOffset + info->entriesOffset, (ut8 *)regular_entries, "iii",
				    info->entriesCount) != entries_size) {
				RZ_LOG_ERROR("Cannot read cache_locsym_entry_t\n");
				goto beach;
			}
			entries = regular_entries;
		}
		RzDyldLocSym *locsym = RZ_NEW0(RzDyldLocSym);
		if (!locsym) {
			goto beach;
		}

		match_bin_entries(cache, entries);

		locsym->local_symbols_offset = hdr->localSymbolsOffset;
		locsym->nlists_offset = info->nlistOffset;
		locsym->nlists_count = info->nlistCount;
		locsym->strings_offset = info->stringsOffset;
		locsym->strings_size = info->stringsSize;

		free(info);
		free(entries);

		return locsym;

	beach:
		free(info);
		free(entries);

		RZ_LOG_ERROR("dyldcache: malformed local symbols metadata\n");
		break;
	}
	return NULL;
}