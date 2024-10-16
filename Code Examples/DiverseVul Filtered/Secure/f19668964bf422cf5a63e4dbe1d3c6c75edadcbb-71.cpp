GF_Err ahdr_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_AdobeDRMHeaderBox *ptr = (GF_AdobeDRMHeaderBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_APRM:
		BOX_FIELD_ASSIGN(std_enc_params, GF_AdobeStdEncryptionParamsBox)
		break;
	}
	return GF_OK;
}