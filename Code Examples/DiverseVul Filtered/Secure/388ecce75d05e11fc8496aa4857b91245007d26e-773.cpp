GF_Err tkhd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TrackHeaderBox *ptr = (GF_TrackHeaderBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->version == 1) {
		gf_bs_write_u64(bs, ptr->creationTime);
		gf_bs_write_u64(bs, ptr->modificationTime);
		gf_bs_write_u32(bs, ptr->trackID);
		gf_bs_write_u32(bs, ptr->reserved1);
		gf_bs_write_u64(bs, ptr->duration);
	} else {
		gf_bs_write_u32(bs, (u32) ptr->creationTime);
		gf_bs_write_u32(bs, (u32) ptr->modificationTime);
		gf_bs_write_u32(bs, ptr->trackID);
		gf_bs_write_u32(bs, ptr->reserved1);
		gf_bs_write_u32(bs, (u32) ptr->duration);
	}
	gf_bs_write_u32(bs, ptr->reserved2[0]);
	gf_bs_write_u32(bs, ptr->reserved2[1]);
	gf_bs_write_u16(bs, ptr->layer);
	gf_bs_write_u16(bs, ptr->alternate_group);
	gf_bs_write_u16(bs, ptr->volume);
	gf_bs_write_u16(bs, ptr->reserved3);
	gf_bs_write_u32(bs, ptr->matrix[0]);
	gf_bs_write_u32(bs, ptr->matrix[1]);
	gf_bs_write_u32(bs, ptr->matrix[2]);
	gf_bs_write_u32(bs, ptr->matrix[3]);
	gf_bs_write_u32(bs, ptr->matrix[4]);
	gf_bs_write_u32(bs, ptr->matrix[5]);
	gf_bs_write_u32(bs, ptr->matrix[6]);
	gf_bs_write_u32(bs, ptr->matrix[7]);
	gf_bs_write_u32(bs, ptr->matrix[8]);
	gf_bs_write_u32(bs, ptr->width);
	gf_bs_write_u32(bs, ptr->height);
	return GF_OK;
}