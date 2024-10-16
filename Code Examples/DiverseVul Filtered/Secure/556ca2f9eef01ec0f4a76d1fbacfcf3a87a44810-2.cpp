static RzList *create_cache_bins(RzDyldCache *cache) {
	RzList *bins = rz_list_newf((RzListFree)free_bin);
	if (!bins) {
		return NULL;
	}

	char *target_libs = NULL;
	RzList *target_lib_names = NULL;
	int *deps = NULL;
	target_libs = rz_sys_getenv("RZ_DYLDCACHE_FILTER");
	if (target_libs) {
		target_lib_names = rz_str_split_list(target_libs, ":", 0);
		if (!target_lib_names) {
			rz_list_free(bins);
			return NULL;
		}
		deps = RZ_NEWS0(int, cache->hdr->imagesCount);
		if (!deps) {
			rz_list_free(bins);
			rz_list_free(target_lib_names);
			return NULL;
		}
	}

	ut32 i;
	for (i = 0; i < cache->n_hdr; i++) {
		cache_hdr_t *hdr = &cache->hdr[i];
		ut64 hdr_offset = cache->hdr_offset[i];
		ut64 symbols_off = cache->symbols_off_base - hdr_offset;
		ut32 maps_index = cache->maps_index[i];
		cache_img_t *img = read_cache_images(cache->buf, hdr, hdr_offset);
		if (!img) {
			goto next;
		}

		ut32 j;
		ut16 *depArray = NULL;
		cache_imgxtr_t *extras = NULL;
		if (target_libs) {
			HtPU *path_to_idx = NULL;
			if (cache->accel) {
				depArray = RZ_NEWS0(ut16, cache->accel->depListCount);
				if (!depArray) {
					goto next;
				}

				if (rz_buf_fread_at(cache->buf, cache->accel->depListOffset, (ut8 *)depArray, "s", cache->accel->depListCount) != cache->accel->depListCount * 2) {
					goto next;
				}

				extras = read_cache_imgextra(cache->buf, hdr, cache->accel);
				if (!extras) {
					goto next;
				}
			} else {
				path_to_idx = create_path_to_index(cache->buf, img, hdr);
			}

			for (j = 0; j < hdr->imagesCount; j++) {
				bool printing = !deps[j];
				char *lib_name = get_lib_name(cache->buf, &img[j]);
				if (!lib_name) {
					break;
				}
				if (strstr(lib_name, "libobjc.A.dylib")) {
					deps[j]++;
				}
				if (!rz_list_find(target_lib_names, lib_name, string_contains)) {
					RZ_FREE(lib_name);
					continue;
				}
				if (printing) {
					RZ_LOG_INFO("FILTER: %s\n", lib_name);
				}
				RZ_FREE(lib_name);
				deps[j]++;

				if (extras && depArray) {
					ut32 k;
					for (k = extras[j].dependentsStartArrayIndex; depArray[k] != 0xffff; k++) {
						ut16 dep_index = depArray[k] & 0x7fff;
						deps[dep_index]++;

						char *dep_name = get_lib_name(cache->buf, &img[dep_index]);
						if (!dep_name) {
							break;
						}
						if (printing) {
							RZ_LOG_INFO("-> %s\n", dep_name);
						}
						free(dep_name);
					}
				} else if (path_to_idx) {
					carve_deps_at_address(cache, img, path_to_idx, img[j].address, deps, printing);
				}
			}

			ht_pu_free(path_to_idx);
			RZ_FREE(depArray);
			RZ_FREE(extras);
		}

		for (j = 0; j < hdr->imagesCount; j++) {
			if (deps && !deps[j]) {
				continue;
			}
			ut64 pa = va2pa(img[j].address, hdr->mappingCount, &cache->maps[maps_index], cache->buf, 0, NULL, NULL);
			if (pa == UT64_MAX) {
				continue;
			}
			ut8 magicbytes[4];
			rz_buf_read_at(cache->buf, pa, magicbytes, 4);
			int magic = rz_read_le32(magicbytes);
			switch (magic) {
			case MH_MAGIC_64: {
				char file[256];
				RzDyldBinImage *bin = RZ_NEW0(RzDyldBinImage);
				if (!bin) {
					goto next;
				}
				bin->header_at = pa;
				bin->hdr_offset = hdr_offset;
				bin->symbols_off = symbols_off;
				bin->va = img[j].address;
				if (rz_buf_read_at(cache->buf, img[j].pathFileOffset, (ut8 *)&file, sizeof(file)) == sizeof(file)) {
					file[255] = 0;
					char *last_slash = strrchr(file, '/');
					if (last_slash && *last_slash) {
						if (last_slash > file) {
							char *scan = last_slash - 1;
							while (scan > file && *scan != '/') {
								scan--;
							}
							if (*scan == '/') {
								bin->file = strdup(scan + 1);
							} else {
								bin->file = strdup(last_slash + 1);
							}
						} else {
							bin->file = strdup(last_slash + 1);
						}
					} else {
						bin->file = strdup(file);
					}
				}
				rz_list_append(bins, bin);
				break;
			}
			default:
				RZ_LOG_WARN("Unknown sub-bin\n");
				break;
			}
		}
	next:
		RZ_FREE(depArray);
		RZ_FREE(extras);
		RZ_FREE(img);
	}
	if (rz_list_empty(bins)) {
		rz_list_free(bins);
		bins = NULL;
	}
	RZ_FREE(deps);
	RZ_FREE(target_libs);
	rz_list_free(target_lib_names);
	return bins;
}