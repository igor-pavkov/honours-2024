GF_Err gf_isom_wma_enum_tag(GF_ISOFile *mov, u32 idx, char **out_tag, const u8 **data, u32 *data_len, u32 *version, u32 *data_type)
{
	GF_XtraBox *xtra;
	GF_XtraTag *tag;

	*out_tag = NULL;
	*data = NULL;
	*data_len = 0;
	*version = 0;
	*data_type = 0;

	xtra = (GF_XtraBox *) gf_isom_get_meta_extensions(mov, GF_TRUE);
	if (!xtra) return GF_URL_ERROR;

	tag = gf_list_get(xtra->tags, idx);
	if (!tag) return GF_NOT_FOUND;
	*out_tag = tag->name;
	*data_len = tag->prop_size;
	*data = tag->prop_value;
	*version = tag->flags;
	*data_type = tag->prop_type;
	return GF_OK;
}