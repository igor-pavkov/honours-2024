GF_Err schi_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_SchemeInformationBox *ptr = (GF_SchemeInformationBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_IKMS:
		BOX_FIELD_ASSIGN(ikms, GF_ISMAKMSBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_ISFM:
		BOX_FIELD_ASSIGN(isfm, GF_ISMASampleFormatBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_ISLT:
		BOX_FIELD_ASSIGN(islt, GF_ISMACrypSaltBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_ODKM:
		BOX_FIELD_ASSIGN(odkm, GF_OMADRMKMSBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_TENC:
		BOX_FIELD_ASSIGN(tenc, GF_TrackEncryptionBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_ADKM:
		BOX_FIELD_ASSIGN(adkm, GF_AdobeDRMKeyManagementSystemBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_UUID:
		if (((GF_UUIDBox*)a)->internal_4cc==GF_ISOM_BOX_UUID_TENC) {
			BOX_FIELD_ASSIGN(piff_tenc, GF_PIFFTrackEncryptionBox)
			return GF_OK;
		} else {
			return GF_OK;
		}
	}
	return GF_OK;
}