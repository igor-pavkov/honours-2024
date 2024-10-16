static cache_img_t *read_cache_images(RzBuffer *cache_buf, cache_hdr_t *hdr, ut64 hdr_offset) {
	if (!cache_buf || !hdr) {
		return NULL;
	}
	if (!hdr->imagesCount || !hdr->imagesOffset || hdr->imagesOffset == UT32_MAX || hdr->imagesCount == UT32_MAX) {
		return NULL;
	}

	ut64 size = sizeof(cache_img_t) * hdr->imagesCount;
	cache_img_t *images = RZ_NEWS0(cache_img_t, hdr->imagesCount);
	if (!images) {
		return NULL;
	}

	if (rz_buf_fread_at(cache_buf, hdr->imagesOffset, (ut8 *)images, "3l2i", hdr->imagesCount) != size) {
		RZ_FREE(images);
		return NULL;
	}

	if (hdr_offset) {
		ut32 i;
		for (i = 0; i < hdr->imagesCount; i++) {
			cache_img_t *img = &images[i];
			img->pathFileOffset += hdr_offset;
		}
	}

	return images;
}