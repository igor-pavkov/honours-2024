GF_Err chan_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_ChannelLayoutInfoBox *ptr = (GF_ChannelLayoutInfoBox *)s;

	ISOM_DECREASE_SIZE(s, 12);
	ptr->layout_tag = gf_bs_read_u32(bs);
	ptr->bitmap = gf_bs_read_u32(bs);
	ptr->num_audio_description = gf_bs_read_u32(bs);

	ptr->audio_descs = gf_malloc(sizeof(GF_AudioChannelDescription) * ptr->num_audio_description);
	for (i=0; i<ptr->num_audio_description; i++) {
		GF_AudioChannelDescription *adesc = &ptr->audio_descs[i];
		ISOM_DECREASE_SIZE(s, 20);
		adesc->label = gf_bs_read_u32(bs);
		adesc->flags = gf_bs_read_u32(bs);
		adesc->coordinates[0] = gf_bs_read_float(bs);
		adesc->coordinates[1] = gf_bs_read_float(bs);
		adesc->coordinates[2] = gf_bs_read_float(bs);
	}
	//avoids warning on most files
	if (ptr->size==20) {
		ptr->size=0;
		gf_bs_skip_bytes(bs, 20);
	}
	return GF_OK;
}