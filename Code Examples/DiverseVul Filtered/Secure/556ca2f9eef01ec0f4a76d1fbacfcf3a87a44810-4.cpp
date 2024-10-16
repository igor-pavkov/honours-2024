static HtPU *create_path_to_index(RzBuffer *cache_buf, cache_img_t *img, cache_hdr_t *hdr) {
	HtPU *path_to_idx = ht_pu_new0();
	if (!path_to_idx) {
		return NULL;
	}
	for (size_t i = 0; i != hdr->imagesCount; i++) {
		char file[256];
		if (rz_buf_read_at(cache_buf, img[i].pathFileOffset, (ut8 *)&file, sizeof(file)) != sizeof(file)) {
			continue;
		}
		file[255] = 0;
		ht_pu_insert(path_to_idx, file, (ut64)i);
	}

	return path_to_idx;
}