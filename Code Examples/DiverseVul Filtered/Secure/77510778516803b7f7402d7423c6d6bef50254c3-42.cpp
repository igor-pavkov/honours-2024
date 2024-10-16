GF_Err trun_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_TrackFragmentRunBox *ptr = (GF_TrackFragmentRunBox *)s;

#ifdef GF_ENABLE_CTRN
	if (ptr->type == GF_ISOM_BOX_TYPE_CTRN) {
		ptr->type = GF_ISOM_BOX_TYPE_TRUN;
		ptr->use_ctrn = GF_TRUE;
		return ctrn_box_read(s, bs);
	}
#endif

	//check this is a good file
	if ((ptr->flags & GF_ISOM_TRUN_FIRST_FLAG) && (ptr->flags & GF_ISOM_TRUN_FLAGS))
		return GF_ISOM_INVALID_FILE;

	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->sample_count = gf_bs_read_u32(bs);

	//The rest depends on the flags
	if (ptr->flags & GF_ISOM_TRUN_DATA_OFFSET) {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->data_offset = gf_bs_read_u32(bs);
	}
	if (ptr->flags & GF_ISOM_TRUN_FIRST_FLAG) {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->first_sample_flags = gf_bs_read_u32(bs);
	}
	if (! (ptr->flags & (GF_ISOM_TRUN_DURATION | GF_ISOM_TRUN_SIZE | GF_ISOM_TRUN_FLAGS | GF_ISOM_TRUN_CTS_OFFSET) ) ) {
		ptr->samples = gf_malloc(sizeof(GF_TrunEntry));
		if (!ptr->samples) return GF_OUT_OF_MEM;
		//memset to 0 !!
		memset(ptr->samples, 0, sizeof(GF_TrunEntry));
		ptr->sample_alloc = ptr->nb_samples = 1;
		ptr->samples[0].nb_pack = ptr->sample_count;
	} else {
		//if we get here, at least one flag (so at least 4 bytes) is set, check size
		if (ptr->sample_count * 4 > ptr->size) {
			ISOM_DECREASE_SIZE(ptr, ptr->sample_count*4);
		}
		if ((u64)ptr->sample_count > (u64)SIZE_MAX/sizeof(GF_TrunEntry)) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of samples %d in trun\n", ptr->sample_count));
			return GF_ISOM_INVALID_FILE;
		}
		ptr->samples = gf_malloc(sizeof(GF_TrunEntry) * ptr->sample_count);
		if (!ptr->samples) return GF_OUT_OF_MEM;
		ptr->sample_alloc = ptr->nb_samples = ptr->sample_count;
		//memset to 0 upfront
		memset(ptr->samples, 0, ptr->sample_count * sizeof(GF_TrunEntry));

		//read each entry (even though nothing may be written)
		for (i=0; i<ptr->sample_count; i++) {
			u32 trun_size = 0;
			GF_TrunEntry *p = &ptr->samples[i];

			if (ptr->flags & GF_ISOM_TRUN_DURATION) {
				p->Duration = gf_bs_read_u32(bs);
				trun_size += 4;
			}
			if (ptr->flags & GF_ISOM_TRUN_SIZE) {
				p->size = gf_bs_read_u32(bs);
				trun_size += 4;
			}
			//SHOULDN'T BE USED IF GF_ISOM_TRUN_FIRST_FLAG IS DEFINED
			if (ptr->flags & GF_ISOM_TRUN_FLAGS) {
				p->flags = gf_bs_read_u32(bs);
				trun_size += 4;
			}
			if (ptr->flags & GF_ISOM_TRUN_CTS_OFFSET) {
				if (ptr->version==0) {
					p->CTS_Offset = (u32) gf_bs_read_u32(bs);
				} else {
					p->CTS_Offset = (s32) gf_bs_read_u32(bs);
				}
				trun_size += 4;
			}
			ISOM_DECREASE_SIZE(ptr, trun_size);
		}
	}
	/*todo parse sample reorder*/
	if (ptr->size) {
		gf_bs_skip_bytes(bs, ptr->size);
		ptr->size = 0;
	}
	return GF_OK;
}