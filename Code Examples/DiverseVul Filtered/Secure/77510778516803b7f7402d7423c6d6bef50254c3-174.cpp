GF_Err trun_box_size(GF_Box *s)
{
	GF_TrackFragmentRunBox *ptr = (GF_TrackFragmentRunBox *)s;

#ifdef GF_ENABLE_CTRN
	if (ptr->use_ctrn)
		return ctrn_box_size(ptr);
#endif

	ptr->size += 4;
	//The rest depends on the flags
	if (ptr->flags & GF_ISOM_TRUN_DATA_OFFSET) ptr->size += 4;
	if (ptr->flags & GF_ISOM_TRUN_FIRST_FLAG) ptr->size += 4;

	if (ptr->sample_order) {
		u32 nb_bytes = 1;
		if (ptr->sample_count>0xFFFFFF) nb_bytes = 4;
		else if (ptr->sample_count>0xFFFF) nb_bytes = 3;
		else if (ptr->sample_count>0xFF) nb_bytes = 2;
		ptr->size += ptr->sample_count*nb_bytes;
	}

	if (! (ptr->flags & (GF_ISOM_TRUN_DURATION | GF_ISOM_TRUN_SIZE | GF_ISOM_TRUN_FLAGS | GF_ISOM_TRUN_CTS_OFFSET) ) ) {
		return GF_OK;
	}

	//if nothing to do, this will be skipped automatically
	if (ptr->flags & GF_ISOM_TRUN_DURATION) ptr->size += 4*ptr->nb_samples;
	if (ptr->flags & GF_ISOM_TRUN_SIZE) ptr->size += 4*ptr->nb_samples;
	//SHOULDN'T BE USED IF GF_ISOM_TRUN_FIRST_FLAG IS DEFINED
	if (ptr->flags & GF_ISOM_TRUN_FLAGS) ptr->size += 4*ptr->nb_samples;
	if (ptr->flags & GF_ISOM_TRUN_CTS_OFFSET) ptr->size += 4*ptr->nb_samples;

	return GF_OK;
}