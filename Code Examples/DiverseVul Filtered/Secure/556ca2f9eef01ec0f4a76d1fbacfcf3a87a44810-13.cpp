static cache_imgxtr_t *read_cache_imgextra(RzBuffer *cache_buf, cache_hdr_t *hdr, cache_accel_t *accel) {
	if (!cache_buf || !hdr || !hdr->imagesCount || !accel || !accel->imageExtrasCount || !accel->imagesExtrasOffset) {
		return NULL;
	}

	ut64 size = sizeof(cache_imgxtr_t) * accel->imageExtrasCount;
	cache_imgxtr_t *images = RZ_NEWS0(cache_imgxtr_t, accel->imageExtrasCount);
	if (!images) {
		return NULL;
	}

	if (rz_buf_fread_at(cache_buf, accel->imagesExtrasOffset, (ut8 *)images, "ll4i", accel->imageExtrasCount) != size) {
		RZ_FREE(images);
		return NULL;
	}

	return images;
}