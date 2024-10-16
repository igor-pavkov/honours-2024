GF_Err tmcd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TimeCodeSampleEntryBox *ptr = (GF_TimeCodeSampleEntryBox *)s;
	GF_Err e = gf_isom_base_sample_entry_read((GF_SampleEntryBox *)s, bs);
	if (e) return e;

	ISOM_DECREASE_SIZE(s, 26);
	gf_bs_read_u32(bs); //reserved
	ptr->flags = gf_bs_read_u32(bs);
	ptr->timescale = gf_bs_read_u32(bs);
	ptr->frame_duration = gf_bs_read_u32(bs);
	ptr->frames_per_sec = gf_bs_read_u8(bs);
	gf_bs_read_u8(bs); //reserved

	return gf_isom_box_array_read(s, bs, NULL);
}