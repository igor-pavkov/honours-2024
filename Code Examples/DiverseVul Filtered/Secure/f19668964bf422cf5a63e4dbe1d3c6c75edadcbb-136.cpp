GF_Box *piff_tenc_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_PIFFTrackEncryptionBox, GF_ISOM_BOX_TYPE_UUID);
	tmp->internal_4cc = GF_ISOM_BOX_UUID_TENC;
	return (GF_Box *)tmp;
}