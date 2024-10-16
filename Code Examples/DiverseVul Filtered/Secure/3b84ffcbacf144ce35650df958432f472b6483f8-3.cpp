GF_SampleEncryptionBox * gf_isom_create_piff_psec_box(u8 version, u32 flags, u32 AlgorithmID, u8 IV_size, bin128 KID)
{
	GF_SampleEncryptionBox *psec;

	psec = (GF_SampleEncryptionBox *) gf_isom_box_new(GF_ISOM_BOX_UUID_PSEC);
	if (!psec)
		return NULL;
	psec->version = version;
	psec->flags = flags;
	psec->piff_type = 1;
	if (psec->flags & 0x1) {
		psec->AlgorithmID = AlgorithmID;
		psec->IV_size = IV_size;
		strcpy((char *)psec->KID, (const char *)KID);
	}
	psec->samp_aux_info = gf_list_new();

	return psec;
}