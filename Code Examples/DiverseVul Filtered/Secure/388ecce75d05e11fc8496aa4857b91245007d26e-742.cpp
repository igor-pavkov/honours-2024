GF_Err rtpo_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_RTPOBox *ptr = (GF_RTPOBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;

	//here we have no pb, just remembed that some entries will have to
	//be 4-bytes aligned ...
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->timeOffset);
	return GF_OK;
}