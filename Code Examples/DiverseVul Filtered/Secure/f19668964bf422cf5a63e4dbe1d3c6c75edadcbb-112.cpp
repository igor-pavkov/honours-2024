GF_Err odkm_Add(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_OMADRMKMSBox *ptr = (GF_OMADRMKMSBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_OHDR:
		BOX_FIELD_ASSIGN(hdr, GF_OMADRMCommonHeaderBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_ODAF:
		BOX_FIELD_ASSIGN(fmt, GF_OMADRMAUFormatBox)
		return GF_OK;
	}
	return GF_OK;
}