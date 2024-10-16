u64 gf_isom_segment_get_fragment_size(GF_ISOFile *file, u32 moof_index, u32 *moof_size)
{
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (!file) return 0;
	u32 moof_sn=0;
	Bool moof_after_mdat = GF_FALSE;
	u64 size=0;
	u32 i, count = gf_list_count(file->TopBoxes);
	if (moof_size) *moof_size = 0;
	for (i=0; i<count; i++) {
		GF_Box *b = gf_list_get(file->TopBoxes, i);
		size += b->size;
		if (b->type == GF_ISOM_BOX_TYPE_MOOF) {
			if (!moof_after_mdat && (moof_sn == moof_index))
				return size - b->size;

			if (moof_size) *moof_size = (u32) b->size;
			moof_sn++;
			if (moof_after_mdat && (moof_sn == moof_index))
				return size;
			if (moof_after_mdat) size=0;
			if ((moof_sn>1) && !moof_after_mdat) size = b->size;
		}
		if (b->type == GF_ISOM_BOX_TYPE_MDAT) {
			if (!moof_sn) moof_after_mdat = GF_TRUE;
		}
	}
	return size;
#endif
	return 0;
}