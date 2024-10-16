GF_MetaBox *gf_isom_apple_create_meta_extensions(GF_ISOFile *mov)
{
	GF_Err e;
	u32 i;
	GF_MetaBox *meta;
	GF_UserDataMap *map;

	if (!mov || !mov->moov) return NULL;

	if (!mov->moov->udta) {
		e = moov_on_child_box((GF_Box*)mov->moov, gf_isom_box_new_parent(&mov->moov->child_boxes, GF_ISOM_BOX_TYPE_UDTA));
		if (e) return NULL;
	}

	map = udta_getEntry(mov->moov->udta, GF_ISOM_BOX_TYPE_META, NULL);
	if (map) {
		for(i = 0; i < gf_list_count(map->boxes); i++) {
			meta = (GF_MetaBox*)gf_list_get(map->boxes, i);

			if(meta != NULL && meta->handler != NULL && meta->handler->handlerType == GF_ISOM_HANDLER_TYPE_MDIR) return meta;
		}
	}

	//udta handles children boxes through maps
	meta = (GF_MetaBox *)gf_isom_box_new(GF_ISOM_BOX_TYPE_META);

	if(meta != NULL) {
		meta->handler = (GF_HandlerBox *)gf_isom_box_new_parent(&meta->child_boxes, GF_ISOM_BOX_TYPE_HDLR);
		if(meta->handler == NULL) {
			gf_isom_box_del((GF_Box *)meta);
			return NULL;
		}
		meta->handler->handlerType = GF_ISOM_HANDLER_TYPE_MDIR;
		gf_isom_box_new_parent(&meta->child_boxes, GF_ISOM_BOX_TYPE_ILST);
		udta_on_child_box((GF_Box *)mov->moov->udta, (GF_Box *)meta);
	}

	return meta;
}