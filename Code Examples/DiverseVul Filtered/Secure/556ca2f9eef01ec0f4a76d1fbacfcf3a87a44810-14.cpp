static RzDyldRebaseInfo *get_rebase_info(RzDyldCache *cache, ut64 slideInfoOffset, ut64 slideInfoSize, ut64 start_of_data, ut64 slide) {
	ut8 *tmp_buf_1 = NULL;
	ut8 *tmp_buf_2 = NULL;
	ut8 *one_page_buf = NULL;
	RzBuffer *cache_buf = cache->buf;

	ut64 offset = slideInfoOffset;
	ut32 slide_info_version = 0;
	if (rz_buf_read_at(cache_buf, offset, (ut8 *)&slide_info_version, 4) != 4) {
		return NULL;
	}

	if (slide_info_version == 3) {
		cache_slide3_t slide_info;
		ut64 size = sizeof(cache_slide3_t);
		if (rz_buf_fread_at(cache_buf, offset, (ut8 *)&slide_info, "4i1l", 1) < 20) {
			return NULL;
		}

		ut64 page_starts_offset = offset + size;
		ut64 page_starts_size = slide_info.page_starts_count * 2;

		if (page_starts_size + size > slideInfoSize) {
			return NULL;
		}

		if (page_starts_size > 0) {
			tmp_buf_1 = malloc(page_starts_size);
			if (!tmp_buf_1) {
				goto beach;
			}
			if (rz_buf_fread_at(cache_buf, page_starts_offset, tmp_buf_1, "s", slide_info.page_starts_count) != page_starts_size) {
				goto beach;
			}
		}

		if (slide_info.page_size > 0) {
			one_page_buf = malloc(slide_info.page_size);
			if (!one_page_buf) {
				goto beach;
			}
		}

		RzDyldRebaseInfo3 *rebase_info = RZ_NEW0(RzDyldRebaseInfo3);
		if (!rebase_info) {
			goto beach;
		}

		rebase_info->version = 3;
		rebase_info->delta_mask = 0x3ff8000000000000ULL;
		rebase_info->delta_shift = 51;
		rebase_info->start_of_data = start_of_data;
		rebase_info->page_starts = (ut16 *)tmp_buf_1;
		rebase_info->page_starts_count = slide_info.page_starts_count;
		rebase_info->auth_value_add = slide_info.auth_value_add;
		rebase_info->page_size = slide_info.page_size;
		rebase_info->one_page_buf = one_page_buf;
		if (slide == UT64_MAX) {
			rebase_info->slide = estimate_slide(cache, 0x7ffffffffffffULL, 0);
			if (rebase_info->slide) {
				RZ_LOG_INFO("dyldcache is slid: 0x%" PFMT64x "\n", rebase_info->slide);
			}
		} else {
			rebase_info->slide = slide;
		}

		return (RzDyldRebaseInfo *)rebase_info;
	} else if (slide_info_version == 2 || slide_info_version == 4) {
		cache_slide2_t slide_info;
		ut64 size = sizeof(cache_slide2_t);
		if (rz_buf_fread_at(cache_buf, offset, (ut8 *)&slide_info, "6i2l", 1) != size) {
			return NULL;
		}

		if (slide_info.page_starts_offset == 0 ||
			slide_info.page_starts_offset > slideInfoSize ||
			slide_info.page_starts_offset + slide_info.page_starts_count * 2 > slideInfoSize) {
			return NULL;
		}

		if (slide_info.page_extras_offset == 0 ||
			slide_info.page_extras_offset > slideInfoSize ||
			slide_info.page_extras_offset + slide_info.page_extras_count * 2 > slideInfoSize) {
			return NULL;
		}

		if (slide_info.page_starts_count > 0) {
			ut64 size = slide_info.page_starts_count * 2;
			ut64 at = slideInfoOffset + slide_info.page_starts_offset;
			tmp_buf_1 = malloc(size);
			if (!tmp_buf_1) {
				goto beach;
			}
			if (rz_buf_fread_at(cache_buf, at, tmp_buf_1, "s", slide_info.page_starts_count) != size) {
				goto beach;
			}
		}

		if (slide_info.page_extras_count > 0) {
			ut64 size = slide_info.page_extras_count * 2;
			ut64 at = slideInfoOffset + slide_info.page_extras_offset;
			tmp_buf_2 = malloc(size);
			if (!tmp_buf_2) {
				goto beach;
			}
			if (rz_buf_fread_at(cache_buf, at, tmp_buf_2, "s", slide_info.page_extras_count) != size) {
				goto beach;
			}
		}

		if (slide_info.page_size > 0) {
			one_page_buf = malloc(slide_info.page_size);
			if (!one_page_buf) {
				goto beach;
			}
		}

		RzDyldRebaseInfo2 *rebase_info = RZ_NEW0(RzDyldRebaseInfo2);
		if (!rebase_info) {
			goto beach;
		}

		rebase_info->version = slide_info_version;
		rebase_info->start_of_data = start_of_data;
		rebase_info->page_starts = (ut16 *)tmp_buf_1;
		rebase_info->page_starts_count = slide_info.page_starts_count;
		rebase_info->page_extras = (ut16 *)tmp_buf_2;
		rebase_info->page_extras_count = slide_info.page_extras_count;
		rebase_info->value_add = slide_info.value_add;
		rebase_info->delta_mask = slide_info.delta_mask;
		rebase_info->value_mask = ~rebase_info->delta_mask;
		rebase_info->delta_shift = dumb_ctzll(rebase_info->delta_mask) - 2;
		rebase_info->page_size = slide_info.page_size;
		rebase_info->one_page_buf = one_page_buf;
		if (slide == UT64_MAX) {
			rebase_info->slide = estimate_slide(cache, rebase_info->value_mask, rebase_info->value_add);
			if (rebase_info->slide) {
				RZ_LOG_INFO("dyldcache is slid: 0x%" PFMT64x "\n", rebase_info->slide);
			}
		} else {
			rebase_info->slide = slide;
		}

		return (RzDyldRebaseInfo *)rebase_info;
	} else if (slide_info_version == 1) {
		cache_slide1_t slide_info;
		ut64 size = sizeof(cache_slide1_t);
		if (rz_buf_fread_at(cache_buf, offset, (ut8 *)&slide_info, "6i", 1) != size) {
			return NULL;
		}

		if (slide_info.toc_offset == 0 ||
			slide_info.toc_offset > slideInfoSize ||
			slide_info.toc_offset + slide_info.toc_count * 2 > slideInfoSize) {
			return NULL;
		}

		if (slide_info.entries_offset == 0 ||
			slide_info.entries_offset > slideInfoSize ||
			slide_info.entries_offset + slide_info.entries_count * slide_info.entries_size > slideInfoSize) {
			return NULL;
		}

		if (slide_info.toc_count > 0) {
			ut64 size = slide_info.toc_count * 2;
			ut64 at = slideInfoOffset + slide_info.toc_offset;
			tmp_buf_1 = malloc(size);
			if (!tmp_buf_1) {
				goto beach;
			}
			if (rz_buf_fread_at(cache_buf, at, tmp_buf_1, "s", slide_info.toc_count) != size) {
				goto beach;
			}
		}

		if (slide_info.entries_count > 0) {
			ut64 size = (ut64)slide_info.entries_count * (ut64)slide_info.entries_size;
			ut64 at = slideInfoOffset + slide_info.entries_offset;
			tmp_buf_2 = malloc(size);
			if (!tmp_buf_2) {
				goto beach;
			}
			if (rz_buf_read_at(cache_buf, at, tmp_buf_2, size) != size) {
				goto beach;
			}
		}

		one_page_buf = malloc(4096);
		if (!one_page_buf) {
			goto beach;
		}

		RzDyldRebaseInfo1 *rebase_info = RZ_NEW0(RzDyldRebaseInfo1);
		if (!rebase_info) {
			goto beach;
		}

		rebase_info->version = 1;
		rebase_info->start_of_data = start_of_data;
		rebase_info->one_page_buf = one_page_buf;
		rebase_info->page_size = 4096;
		rebase_info->toc = (ut16 *)tmp_buf_1;
		rebase_info->toc_count = slide_info.toc_count;
		rebase_info->entries = tmp_buf_2;
		rebase_info->entries_size = slide_info.entries_size;
		if (slide == UT64_MAX) {
			rebase_info->slide = estimate_slide(cache, UT64_MAX, 0);
			if (rebase_info->slide) {
				RZ_LOG_INFO("dyldcache is slid: 0x%" PFMT64x "\n", rebase_info->slide);
			}
		} else {
			rebase_info->slide = slide;
		}

		return (RzDyldRebaseInfo *)rebase_info;
	} else {
		RZ_LOG_ERROR("Unsupported slide info version %d\n", slide_info_version);
		return NULL;
	}

beach:
	free(tmp_buf_1);
	free(tmp_buf_2);
	free(one_page_buf);
	return NULL;
}