GF_Err fdsa_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_HintSample *ptr = (GF_HintSample *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	e = gf_isom_box_array_write(s, ptr->packetTable, bs);
	if (e) return e;
	if (ptr->extra_data) {
		e = gf_isom_box_write((GF_Box *)ptr->extra_data, bs);
		if (e) return e;
	}
	return GF_OK;
}