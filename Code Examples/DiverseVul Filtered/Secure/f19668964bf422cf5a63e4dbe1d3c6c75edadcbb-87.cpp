GF_Err odrb_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_OMADRMRightsObjectBox *ptr = (GF_OMADRMRightsObjectBox *)s;

	ptr->oma_ro_size = (u32) ptr->size;
	ptr->oma_ro = (char*) gf_malloc(sizeof(char)*ptr->oma_ro_size);
	if (!ptr->oma_ro) return GF_OUT_OF_MEM;
	gf_bs_read_data(bs, ptr->oma_ro, ptr->oma_ro_size);
	ptr->size = 0;
	return GF_OK;
}