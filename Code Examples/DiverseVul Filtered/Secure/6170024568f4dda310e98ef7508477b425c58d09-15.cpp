GF_MetaBox *gf_isom_apple_get_meta_extensions(GF_ISOFile *mov)
{
	u32 i;
	GF_MetaBox *meta;
	GF_UserDataMap *map;

	if (!mov || !mov->moov) return NULL;

	if (!mov->moov->udta) return NULL;
	map = udta_getEntry(mov->moov->udta, GF_ISOM_BOX_TYPE_META, NULL);
	if (!map) return NULL;

	for(i = 0; i < gf_list_count(map->boxes); i++) {
		meta = (GF_MetaBox*)gf_list_get(map->boxes, i);

		if(meta != NULL && meta->handler != NULL && meta->handler->handlerType == GF_ISOM_HANDLER_TYPE_MDIR) return meta;
	}

	return NULL;
}