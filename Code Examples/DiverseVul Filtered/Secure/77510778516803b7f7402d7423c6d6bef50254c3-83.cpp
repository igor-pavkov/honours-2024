GF_Err jp2h_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_J2KHeaderBox *ptr = (GF_J2KHeaderBox *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_IHDR:
		BOX_FIELD_ASSIGN(ihdr, GF_J2KImageHeaderBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_COLR:
		BOX_FIELD_ASSIGN(colr, GF_ColourInformationBox)
		return GF_OK;
	}
	return GF_OK;
}