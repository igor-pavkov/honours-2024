void piff_psec_box_del(GF_Box *s)
{
	GF_SampleEncryptionBox *ptr = (GF_SampleEncryptionBox *)s;
	while (gf_list_count(ptr->samp_aux_info)) {
		GF_CENCSampleAuxInfo *sai = (GF_CENCSampleAuxInfo *)gf_list_get(ptr->samp_aux_info, 0);
		if (sai) gf_isom_cenc_samp_aux_info_del(sai);
		gf_list_rem(ptr->samp_aux_info, 0);
	}
	if (ptr->samp_aux_info) gf_list_del(ptr->samp_aux_info);
	gf_free(s);
}