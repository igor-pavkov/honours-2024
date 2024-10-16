GF_Err prft_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ProducerReferenceTimeBox *ptr = (GF_ProducerReferenceTimeBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->refTrackID);
	gf_bs_write_u64(bs, ptr->ntp);
	if (ptr->version==0) {
		gf_bs_write_u32(bs, (u32) ptr->timestamp);
	} else {
		gf_bs_write_u64(bs, ptr->timestamp);
	}

	return GF_OK;
}