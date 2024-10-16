GF_Err alis_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
//	GF_DataEntryAliasBox *ptr = (GF_DataEntryAliasBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	return GF_OK;
}