GF_Err tref_box_write(GF_Box *s, GF_BitStream *bs)
{
//	GF_TrackReferenceBox *ptr = (GF_TrackReferenceBox *)s;
	return gf_isom_box_write_header(s, bs);
}