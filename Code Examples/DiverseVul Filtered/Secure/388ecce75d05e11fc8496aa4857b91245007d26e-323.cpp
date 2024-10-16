void mdia_box_del(GF_Box *s)
{
	GF_MediaBox *ptr = (GF_MediaBox *)s;
	if (ptr == NULL) return;
	if (ptr->nalu_parser) gf_bs_del(ptr->nalu_parser);
	if (ptr->nalu_out_bs) gf_bs_del(ptr->nalu_out_bs);
	if (ptr->nalu_ps_bs) gf_bs_del(ptr->nalu_ps_bs);
	if (ptr->extracted_bs) gf_bs_del(ptr->extracted_bs);
	if (ptr->extracted_samp) gf_isom_sample_del(&ptr->extracted_samp);
	if (ptr->in_sample_buffer) gf_free(ptr->in_sample_buffer);
	if (ptr->tmp_nal_copy_buffer) gf_free(ptr->tmp_nal_copy_buffer);
	gf_free(ptr);
}