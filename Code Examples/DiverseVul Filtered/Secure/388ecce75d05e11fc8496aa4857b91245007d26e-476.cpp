GF_Err tkhd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TrackHeaderBox *ptr = (GF_TrackHeaderBox *)s;

	if (ptr->version == 1) {
		ISOM_DECREASE_SIZE(ptr, 32);
		ptr->creationTime = gf_bs_read_u64(bs);
		ptr->modificationTime = gf_bs_read_u64(bs);
		ptr->trackID = gf_bs_read_u32(bs);
		ptr->reserved1 = gf_bs_read_u32(bs);
		ptr->duration = gf_bs_read_u64(bs);
	} else {
		ISOM_DECREASE_SIZE(ptr, 20);
		ptr->creationTime = gf_bs_read_u32(bs);
		ptr->modificationTime = gf_bs_read_u32(bs);
		ptr->trackID = gf_bs_read_u32(bs);
		ptr->reserved1 = gf_bs_read_u32(bs);
		ptr->duration = gf_bs_read_u32(bs);
	}
	ptr->initial_duration = ptr->duration;

	ISOM_DECREASE_SIZE(ptr, 60);
	ptr->reserved2[0] = gf_bs_read_u32(bs);
	ptr->reserved2[1] = gf_bs_read_u32(bs);
	ptr->layer = gf_bs_read_u16(bs);
	ptr->alternate_group = gf_bs_read_u16(bs);
	ptr->volume = gf_bs_read_u16(bs);
	ptr->reserved3 = gf_bs_read_u16(bs);
	ptr->matrix[0] = gf_bs_read_u32(bs);
	ptr->matrix[1] = gf_bs_read_u32(bs);
	ptr->matrix[2] = gf_bs_read_u32(bs);
	ptr->matrix[3] = gf_bs_read_u32(bs);
	ptr->matrix[4] = gf_bs_read_u32(bs);
	ptr->matrix[5] = gf_bs_read_u32(bs);
	ptr->matrix[6] = gf_bs_read_u32(bs);
	ptr->matrix[7] = gf_bs_read_u32(bs);
	ptr->matrix[8] = gf_bs_read_u32(bs);
	ptr->width = gf_bs_read_u32(bs);
	ptr->height = gf_bs_read_u32(bs);
	return GF_OK;
}