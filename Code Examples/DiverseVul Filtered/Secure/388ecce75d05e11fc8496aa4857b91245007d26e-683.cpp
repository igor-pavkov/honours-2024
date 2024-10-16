GF_Err moov_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_MovieBox *ptr = (GF_MovieBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_IODS:
		if (ptr->iods) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->iods = (GF_ObjectDescriptorBox *)a;
		//if no IOD, delete the box
		if (!ptr->iods->descriptor) {
			ptr->iods = NULL;
			gf_isom_box_del_parent(&s->child_boxes, a);
		}
		return GF_OK;

	case GF_ISOM_BOX_TYPE_MVHD:
		if (ptr->mvhd) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->mvhd = (GF_MovieHeaderBox *)a;
		return GF_OK;

	case GF_ISOM_BOX_TYPE_UDTA:
		if (ptr->udta) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->udta = (GF_UserDataBox *)a;
		return GF_OK;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	case GF_ISOM_BOX_TYPE_MVEX:
		if (ptr->mvex) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->mvex = (GF_MovieExtendsBox *)a;
		ptr->mvex->mov = ptr->mov;
		return GF_OK;
#endif

	case GF_ISOM_BOX_TYPE_META:
		if (ptr->meta) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->meta = (GF_MetaBox *)a;
		return GF_OK;

	case GF_ISOM_BOX_TYPE_TRAK:
		//set our pointer to this obj
		((GF_TrackBox *)a)->moov = ptr;
		return gf_list_add(ptr->trackList, a);
	}
	return GF_OK;
}