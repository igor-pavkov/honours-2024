GF_Err ssix_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i, j;
	GF_SubsegmentIndexBox *ptr = (GF_SubsegmentIndexBox*)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->subsegment_count);
	for (i = 0; i<ptr->subsegment_count; i++) {
		gf_bs_write_u32(bs, ptr->subsegments[i].range_count);
		for (j = 0; j < ptr->subsegments[i].range_count; j++) {
			gf_bs_write_u8(bs, ptr->subsegments[i].ranges[j].level);
			gf_bs_write_u24(bs, ptr->subsegments[i].ranges[j].range_size);
		}
	}
	return GF_OK;
}