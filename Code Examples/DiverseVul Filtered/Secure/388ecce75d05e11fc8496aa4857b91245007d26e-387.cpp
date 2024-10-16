GF_Err url_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_DataEntryURLBox *ptr = (GF_DataEntryURLBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	//the flag set indicates we have a string (WE HAVE TO for URLs)
	if ( !(ptr->flags & 1)) {
		if (ptr->location) {
			gf_bs_write_data(bs, ptr->location, (u32)strlen(ptr->location) + 1);
		}
	}
	return GF_OK;
}