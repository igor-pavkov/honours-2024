GF_Err tfxd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MSSTimeExtBox *ptr = (GF_MSSTimeExtBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->version = gf_bs_read_u8(bs);
	ptr->flags = gf_bs_read_u24(bs);

	if (ptr->version == 0x01) {
		ISOM_DECREASE_SIZE(ptr, 16);
		ptr->absolute_time_in_track_timescale = gf_bs_read_u64(bs);
		ptr->fragment_duration_in_track_timescale = gf_bs_read_u64(bs);
	} else {
		ISOM_DECREASE_SIZE(ptr, 8);
		ptr->absolute_time_in_track_timescale = gf_bs_read_u32(bs);
		ptr->fragment_duration_in_track_timescale = gf_bs_read_u32(bs);
	}

	return GF_OK;
}