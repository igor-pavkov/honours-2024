GF_Box *tenc_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackEncryptionBox, GF_ISOM_BOX_TYPE_TENC);
	return (GF_Box *)tmp;
}