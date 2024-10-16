GF_Err urn_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_DataEntryURNBox *ptr = (GF_DataEntryURNBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	//the flag set indicates we have a string (WE HAVE TO for URLs)
	if ( !(ptr->flags & 1)) {
		//to check, the spec says: First name, then location
		if (ptr->nameURN) {
			gf_bs_write_data(bs, ptr->nameURN, (u32)strlen(ptr->nameURN) + 1);
		}
		if (ptr->location) {
			gf_bs_write_data(bs, ptr->location, (u32)strlen(ptr->location) + 1);
		}
	}
	return GF_OK;
}