GF_Err sinf_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_ProtectionSchemeInfoBox *ptr = (GF_ProtectionSchemeInfoBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_FRMA:
		BOX_FIELD_ASSIGN(original_format, GF_OriginalFormatBox)
		break;
	case GF_ISOM_BOX_TYPE_SCHM:
		BOX_FIELD_ASSIGN(scheme_type, GF_SchemeTypeBox)
		break;
	case GF_ISOM_BOX_TYPE_SCHI:
		BOX_FIELD_ASSIGN(info, GF_SchemeInformationBox)
		break;
	}
	return GF_OK;
}