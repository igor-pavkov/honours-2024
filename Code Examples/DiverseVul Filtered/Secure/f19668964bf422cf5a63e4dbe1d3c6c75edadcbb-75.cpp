GF_Box *piff_psec_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleEncryptionBox, GF_ISOM_BOX_TYPE_UUID);
	tmp->internal_4cc = GF_ISOM_BOX_UUID_PSEC;
	tmp->piff_type = 1;
	return (GF_Box *)tmp;
}