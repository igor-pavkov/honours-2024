GF_SampleEncryptionBox * gf_isom_create_samp_enc_box(u8 version, u32 flags)
{
	GF_SampleEncryptionBox *senc;

	senc = (GF_SampleEncryptionBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_SENC);
	if (!senc)
		return NULL;
	senc->version = version;
	senc->flags = flags;
	senc->samp_aux_info = gf_list_new();

	return senc;
}