GF_Err ssix_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i,j;
	GF_SubsegmentIndexBox *ptr = (GF_SubsegmentIndexBox*)s;

	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->subsegment_count = gf_bs_read_u32(bs);
	//each subseg has at least one range_count (4 bytes), abort if not enough bytes (broken box)
	if (ptr->size / 4 < ptr->subsegment_count || (u64)ptr->subsegment_count > (u64)SIZE_MAX/sizeof(GF_SubsegmentInfo))
		return GF_ISOM_INVALID_FILE;

	ptr->subsegment_alloc = ptr->subsegment_count;
	GF_SAFE_ALLOC_N(ptr->subsegments, ptr->subsegment_count, GF_SubsegmentInfo);
	if (!ptr->subsegments)
	    return GF_OUT_OF_MEM;
	for (i = 0; i < ptr->subsegment_count; i++) {
		GF_SubsegmentInfo *subseg = &ptr->subsegments[i];
		ISOM_DECREASE_SIZE(ptr, 4)
		subseg->range_count = gf_bs_read_u32(bs);
		//each range is 4 bytes, abort if not enough bytes
		if (ptr->size / 4 < subseg->range_count || (u64)subseg->range_count > (u64)SIZE_MAX/sizeof(GF_SubsegmentRangeInfo))
			return GF_ISOM_INVALID_FILE;
		subseg->ranges = (GF_SubsegmentRangeInfo*) gf_malloc(sizeof(GF_SubsegmentRangeInfo) * subseg->range_count);
		if (!subseg->ranges) return GF_OUT_OF_MEM;
		for (j = 0; j < subseg->range_count; j++) {
			ISOM_DECREASE_SIZE(ptr, 4)
			subseg->ranges[j].level = gf_bs_read_u8(bs);
			subseg->ranges[j].range_size = gf_bs_read_u24(bs);
		}
	}
	return GF_OK;
}