GF_Err chan_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_ChannelLayoutInfoBox *ptr = (GF_ChannelLayoutInfoBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;


	gf_bs_write_u32(bs, ptr->layout_tag);
	gf_bs_write_u32(bs, ptr->bitmap);
	gf_bs_write_u32(bs, ptr->num_audio_description);
	for (i=0; i<ptr->num_audio_description; i++) {
		GF_AudioChannelDescription *adesc = &ptr->audio_descs[i];
		gf_bs_write_u32(bs, adesc->label);
		gf_bs_write_u32(bs, adesc->flags);
		gf_bs_write_float(bs, adesc->coordinates[0]);
		gf_bs_write_float(bs, adesc->coordinates[1]);
		gf_bs_write_float(bs, adesc->coordinates[2]);
	}

	return GF_OK;
}