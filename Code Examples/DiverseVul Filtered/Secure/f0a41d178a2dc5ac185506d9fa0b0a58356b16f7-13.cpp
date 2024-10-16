GF_Err gf_isom_apple_get_tag(GF_ISOFile *mov, GF_ISOiTunesTag tag, const u8 **data, u32 *data_len)
{
	u32 i;
	GF_ListItemBox *info;
	GF_ItemListBox *ilst;
	GF_MetaBox *meta;

	*data = NULL;
	*data_len = 0;

	meta = (GF_MetaBox *) gf_isom_get_meta_extensions(mov, GF_FALSE);
	if (!meta) return GF_URL_ERROR;

	ilst = gf_isom_locate_box(meta->child_boxes, GF_ISOM_BOX_TYPE_ILST, NULL);
	if (!ilst) return GF_URL_ERROR;

	if (tag==GF_ISOM_ITUNE_PROBE) return gf_list_count(ilst->child_boxes) ? GF_OK : GF_URL_ERROR;

	i=0;
	while ( (info=(GF_ListItemBox*)gf_list_enum(ilst->child_boxes, &i))) {
		if (info->type==tag) break;
		/*special cases*/
		if ((tag==GF_ISOM_ITUNE_GENRE) && (info->type==(u32) GF_ISOM_ITUNE_GENRE_USER)) break;
		info = NULL;
	}
	if (!info || !info->data || !info->data->data) return GF_URL_ERROR;

	if ((tag == GF_ISOM_ITUNE_GENRE) && (info->data->flags == 0)) {
		if (info->data->dataSize && (info->data->dataSize>2) && (info->data->dataSize < 5)) {
			GF_BitStream* bs = gf_bs_new(info->data->data, info->data->dataSize, GF_BITSTREAM_READ);
			*data_len = gf_bs_read_int(bs, info->data->dataSize * 8);
			gf_bs_del(bs);
			return GF_OK;
		}
	}
//	if (info->data->flags != 0x1) return GF_URL_ERROR;
	*data = info->data->data;
	*data_len = info->data->dataSize;
	if ((tag==GF_ISOM_ITUNE_COVER_ART) && (info->data->flags==14)) *data_len |= 0x80000000; //(1<<31);
	return GF_OK;
}