GF_Err pcmC_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_PCMConfigBox *ptr = (GF_PCMConfigBox *) s;

	ISOM_DECREASE_SIZE(s, 2)
	ptr->format_flags = gf_bs_read_u8(bs);
	ptr->PCM_sample_size = gf_bs_read_u8(bs);
	return GF_OK;
}