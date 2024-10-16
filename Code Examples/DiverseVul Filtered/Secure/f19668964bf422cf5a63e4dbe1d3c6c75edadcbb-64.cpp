GF_Err aprm_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_AdobeStdEncryptionParamsBox *ptr = (GF_AdobeStdEncryptionParamsBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_AEIB:
		BOX_FIELD_ASSIGN(enc_info, GF_AdobeEncryptionInfoBox)
		break;
	case GF_ISOM_BOX_TYPE_AKEY:
		BOX_FIELD_ASSIGN(key_info, GF_AdobeKeyInfoBox)
		break;
	}
	return GF_OK;
}