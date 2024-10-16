static void populate_cache_headers(RzDyldCache *cache) {
	cache->n_hdr = 0;
	RzList *hdrs = rz_list_newf(NULL);
	if (!hdrs) {
		return;
	}

	cache_hdr_t *h;
	ut64 offsets[MAX_N_HDR];
	ut64 offset = 0;
	do {
		offsets[cache->n_hdr] = offset;
		h = read_cache_header(cache->buf, offset);
		if (!h) {
			break;
		}
		rz_list_append(hdrs, h);

		ut64 size = h->codeSignatureOffset + h->codeSignatureSize;

#define SHIFT_MAYBE(x) \
	if (x) { \
		x += offset; \
	}

		SHIFT_MAYBE(h->mappingOffset);
		SHIFT_MAYBE(h->imagesOffset);
		SHIFT_MAYBE(h->codeSignatureOffset);
		SHIFT_MAYBE(h->slideInfoOffset);
		SHIFT_MAYBE(h->localSymbolsOffset);
		SHIFT_MAYBE(h->branchPoolsOffset);
		SHIFT_MAYBE(h->imagesTextOffset);

		offset += size;
		cache->n_hdr++;
	} while (cache->n_hdr < MAX_N_HDR);

	if (!cache->n_hdr) {
		goto beach;
	}

	cache->hdr = RZ_NEWS0(cache_hdr_t, cache->n_hdr);
	if (!cache->hdr) {
		cache->n_hdr = 0;
		goto beach;
	}

	cache->hdr_offset = RZ_NEWS0(ut64, cache->n_hdr);
	if (!cache->hdr_offset) {
		cache->n_hdr = 0;
		RZ_FREE(cache->hdr);
		goto beach;
	}

	memcpy(cache->hdr_offset, offsets, cache->n_hdr * sizeof(ut64));

	ut32 i = 0;
	RzListIter *iter;
	cache_hdr_t *item;
	rz_list_foreach (hdrs, iter, item) {
		if (i >= cache->n_hdr) {
			break;
		}
		memcpy(&cache->hdr[i++], item, sizeof(cache_hdr_t));
	}

beach:
	rz_list_free(hdrs);
}