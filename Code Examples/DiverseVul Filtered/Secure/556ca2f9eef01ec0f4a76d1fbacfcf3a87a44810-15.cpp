static void match_bin_entries(RzDyldCache *cache, void *entries) {
	rz_return_if_fail(cache && cache->bins && entries);

	cache_img_t *imgs = read_cache_images(cache->buf, cache->hdr, 0);
	if (!imgs) {
		return;
	}

	RzDyldBinImage *bin = NULL;
	RzListIter *it = rz_list_iterator(cache->bins);

	bool has_large_entries = cache->n_hdr > 1;

	ut32 i;
	for (i = 0; i < cache->hdr->imagesCount; i++) {
		cache_img_t *img = &imgs[i];
		if (!it) {
			break;
		}
		bin = it->data;
		if (!bin) {
			break;
		}
		if (bin && bin->va == img->address) {
			if (has_large_entries) {
				cache_locsym_entry_large_t *e = &((cache_locsym_entry_large_t *)entries)[i];
				bin->nlist_start_index = e->nlistStartIndex;
				bin->nlist_count = e->nlistCount;
			} else {
				cache_locsym_entry_t *e = &((cache_locsym_entry_t *)entries)[i];
				bin->nlist_start_index = e->nlistStartIndex;
				bin->nlist_count = e->nlistCount;
			}
			it = it->n;
		}
	}

	RZ_FREE(imgs);
}