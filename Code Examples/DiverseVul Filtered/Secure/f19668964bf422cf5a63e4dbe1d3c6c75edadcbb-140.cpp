GF_Err adkm_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_AdobeDRMKeyManagementSystemBox *ptr = (GF_AdobeDRMKeyManagementSystemBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_AHDR:
		BOX_FIELD_ASSIGN(header, GF_AdobeDRMHeaderBox)
		break;
	case GF_ISOM_BOX_TYPE_ADAF:
		BOX_FIELD_ASSIGN(au_format, GF_AdobeDRMAUFormatBox)
		break;
	}
	return GF_OK;
}