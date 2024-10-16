void gf_isom_cenc_samp_aux_info_del(GF_CENCSampleAuxInfo *samp)
{
	if (!samp) return;
	if (samp->cenc_data) gf_free(samp->cenc_data);
	gf_free(samp);
}