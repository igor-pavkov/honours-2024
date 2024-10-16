GF_Box *senc_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleEncryptionBox, GF_ISOM_BOX_TYPE_SENC);
	return (GF_Box *)tmp;
}