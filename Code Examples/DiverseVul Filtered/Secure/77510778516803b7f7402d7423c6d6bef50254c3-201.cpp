GF_Err ssix_box_size(GF_Box *s)
{
	u32 i;
	GF_SubsegmentIndexBox *ptr = (GF_SubsegmentIndexBox*)s;

	ptr->size += 4;
	for (i = 0; i < ptr->subsegment_count; i++) {
		ptr->size += 4 + 4 * ptr->subsegments[i].range_count;
	}
	return GF_OK;
}