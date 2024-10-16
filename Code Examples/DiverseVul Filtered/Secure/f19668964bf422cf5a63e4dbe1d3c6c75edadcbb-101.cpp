GF_Err akey_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_AdobeKeyInfoBox *ptr = (GF_AdobeKeyInfoBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_FLXS:
		BOX_FIELD_ASSIGN(params, GF_AdobeFlashAccessParamsBox)
		break;
	}
	return GF_OK;
}