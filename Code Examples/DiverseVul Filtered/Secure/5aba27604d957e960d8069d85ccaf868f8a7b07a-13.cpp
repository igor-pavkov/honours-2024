GF_Err DoWriteMeta(GF_ISOFile *file, GF_MetaBox *meta, GF_BitStream *bs, Bool Emulation, u64 baseOffset, u64 *mdatSize)
{
	GF_ItemExtentEntry *entry;
	u64 maxExtendOffset, maxExtendSize;
	u32 i, j, count;

	maxExtendOffset = 0;
	maxExtendSize = 0;
	if (mdatSize) *mdatSize = 0;
	if (!meta->item_locations) return GF_OK;

	count = gf_list_count(meta->item_locations->location_entries);
	for (i=0; i<count; i++) {
		u64 it_size;
		GF_ItemLocationEntry *iloc = (GF_ItemLocationEntry *)gf_list_get(meta->item_locations->location_entries, i);
		/*get item info*/
		GF_ItemInfoEntryBox *iinf = NULL;
		j=0;
		while ((iinf = (GF_ItemInfoEntryBox *)gf_list_enum(meta->item_infos->item_infos, &j))) {
			if (iinf->item_ID==iloc->item_ID) break;
			iinf = NULL;
		}

		if (!iloc->base_offset && (gf_list_count(iloc->extent_entries)==1)) {
			entry = (GF_ItemExtentEntry *)gf_list_get(iloc->extent_entries, 0);
			if (!entry->extent_length && !entry->original_extent_offset && !entry->extent_index) {
				entry->extent_offset = 0;
				continue;
			}
		}

		it_size = 0;
		/*for self contained only*/
		if (!iloc->data_reference_index) {
			if (iloc->construction_method != 2) {
				iloc->base_offset = baseOffset;
			}

			/*new resource*/
			if (iinf && (iinf->full_path || (iinf->tk_id && iinf->sample_num))) {
				FILE *src=NULL;

				if (!iinf->data_len && iinf->full_path) {
					src = gf_fopen(iinf->full_path, "rb");
					if (!src) continue;
					it_size = gf_fsize(src);
				} else {
					it_size = iinf->data_len;
				}
				if (maxExtendSize<it_size) maxExtendSize = it_size;

				if (!gf_list_count(iloc->extent_entries)) {
					GF_SAFEALLOC(entry, GF_ItemExtentEntry);
					if (!entry) return GF_OUT_OF_MEM;
					gf_list_add(iloc->extent_entries, entry);
				}
				entry = (GF_ItemExtentEntry *)gf_list_get(iloc->extent_entries, 0);
				entry->extent_offset = 0;
				entry->extent_length = it_size;

				//shared data, do not count it
				if (iinf->tk_id && iinf->sample_num) {
					it_size = 0;
					maxExtendOffset = 0xFFFFFFFFFFUL;
					if (Emulation) {
						meta->use_item_sample_sharing = GF_TRUE;
					}
				}

				/*OK write to mdat*/
				if (!Emulation) {
					if (iinf->tk_id && iinf->sample_num) {
					}
					else if (src) {
						char cache_data[4096];
						u64 remain = entry->extent_length;
						while (remain) {
							u32 size_cache = (remain>4096) ? 4096 : (u32) remain;
							size_t read = gf_fread(cache_data, size_cache, src);
							if (read ==(size_t) -1) break;
							gf_bs_write_data(bs, cache_data, (u32) read);
							remain -= (u32) read;
						}
					} else {
						gf_bs_write_data(bs, iinf->full_path, iinf->data_len);
					}
				}
				if (src) gf_fclose(src);
			}
			else if (gf_list_count(iloc->extent_entries)) {
				j=0;
				while ((entry = (GF_ItemExtentEntry *)gf_list_enum(iloc->extent_entries, &j))) {
					if (entry->extent_index) continue;
					if (j && (maxExtendOffset<it_size) ) maxExtendOffset = it_size;
					/*compute new offset*/
					entry->extent_offset = baseOffset + it_size;

					it_size += entry->extent_length;
					if (maxExtendSize<entry->extent_length) maxExtendSize = entry->extent_length;

					/*Reading from the input file*/
					if (!Emulation) {
						char cache_data[4096];
						u64 remain = entry->extent_length;
						gf_bs_seek(file->movieFileMap->bs, entry->original_extent_offset + iloc->original_base_offset);
						while (remain) {
							u32 size_cache = (remain>4096) ? 4096 : (u32) remain;
							gf_bs_read_data(file->movieFileMap->bs, cache_data, size_cache);
							/*Writing to the output file*/
							gf_bs_write_data(bs, cache_data, size_cache);
							remain -= size_cache;
						}
					}
				}
			}
			baseOffset += it_size;
			if (mdatSize)
				*mdatSize += it_size;
		} else {
			/*we MUST have at least one extent for the dref data*/
			if (!gf_list_count(iloc->extent_entries)) {
				GF_SAFEALLOC(entry, GF_ItemExtentEntry);
				if (!entry) return GF_OUT_OF_MEM;
				gf_list_add(iloc->extent_entries, entry);
			}
			entry = (GF_ItemExtentEntry *)gf_list_get(iloc->extent_entries, 0);
			entry->extent_offset = 0;
			/*0 means full length of referenced file*/
			entry->extent_length = 0;
		}
	}

	/*update offset & size length fields*/
	if (baseOffset>0xFFFFFFFF) meta->item_locations->base_offset_size = 8;
	else if (baseOffset) meta->item_locations->base_offset_size = 4;

	if (maxExtendSize>0xFFFFFFFF) meta->item_locations->length_size = 8;
	else if (maxExtendSize) meta->item_locations->length_size = 4;

	if (maxExtendOffset>0xFFFFFFFF) meta->item_locations->offset_size = 8;
	else if (maxExtendOffset) meta->item_locations->offset_size = 4;
	return GF_OK;
}