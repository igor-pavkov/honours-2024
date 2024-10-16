GF_Err rvcc_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_RVCConfigurationBox *ptr = (GF_RVCConfigurationBox*)s;
	ISOM_DECREASE_SIZE(ptr, 2);
	ptr->predefined_rvc_config = gf_bs_read_u16(bs);
	if (!ptr->predefined_rvc_config) {
		ISOM_DECREASE_SIZE(ptr, 2);
		ptr->rvc_meta_idx = gf_bs_read_u16(bs);
	}
	return GF_OK;
}