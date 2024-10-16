GF_Err trak_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_TrackBox *ptr = (GF_TrackBox *)s;
	if (!a) return GF_OK;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_TKHD:
		BOX_FIELD_ASSIGN(Header, GF_TrackHeaderBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_EDTS:
		BOX_FIELD_ASSIGN(editBox, GF_EditBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_UDTA:
		BOX_FIELD_ASSIGN(udta, GF_UserDataBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_META:
		BOX_FIELD_ASSIGN(meta, GF_MetaBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_TREF:
		BOX_FIELD_ASSIGN(References, GF_TrackReferenceBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_MDIA:
		BOX_FIELD_ASSIGN(Media, GF_MediaBox)
		if (!is_rem)
			((GF_MediaBox *)a)->mediaTrack = ptr;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_TRGR:
		BOX_FIELD_ASSIGN(groups, GF_TrackGroupBox)
		return GF_OK;
	case GF_QT_BOX_TYPE_TAPT:
		BOX_FIELD_ASSIGN(Aperture, GF_Box)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_SENC:
		BOX_FIELD_ASSIGN(sample_encryption, GF_SampleEncryptionBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_UUID:
		if (((GF_UnknownUUIDBox *)a)->internal_4cc == GF_ISOM_BOX_UUID_PSEC) {
			BOX_FIELD_ASSIGN(sample_encryption, GF_SampleEncryptionBox)
			return GF_OK;
		}
	}
	return GF_OK;
}