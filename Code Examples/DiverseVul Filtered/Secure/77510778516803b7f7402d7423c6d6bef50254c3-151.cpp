GF_Err trep_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TrackExtensionPropertiesBox *ptr = (GF_TrackExtensionPropertiesBox *)s;

	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->trackID = gf_bs_read_u32(bs);

	return gf_isom_box_array_read(s, bs);
}