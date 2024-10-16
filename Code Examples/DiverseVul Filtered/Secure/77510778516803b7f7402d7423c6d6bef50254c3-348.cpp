GF_Err moov_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_MovieBox *ptr = (GF_MovieBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_IODS:
		BOX_FIELD_ASSIGN(iods, GF_ObjectDescriptorBox)
		//if no IOD, delete the box
		if (ptr->iods && !ptr->iods->descriptor) {
			ptr->iods = NULL;
			gf_isom_box_del_parent(&s->child_boxes, a);
		}
		return GF_OK;

	case GF_ISOM_BOX_TYPE_MVHD:
		BOX_FIELD_ASSIGN(mvhd, GF_MovieHeaderBox)
		return GF_OK;

	case GF_ISOM_BOX_TYPE_UDTA:
		BOX_FIELD_ASSIGN(udta, GF_UserDataBox)
		return GF_OK;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	case GF_ISOM_BOX_TYPE_MVEX:
		BOX_FIELD_ASSIGN(mvex, GF_MovieExtendsBox)
		if (ptr->mvex)
			ptr->mvex->mov = ptr->mov;
		return GF_OK;
#endif

	case GF_ISOM_BOX_TYPE_META:
		BOX_FIELD_ASSIGN(meta, GF_MetaBox)
		return GF_OK;

	case GF_ISOM_BOX_TYPE_TRAK:
		if (is_rem) {
			gf_list_del_item(ptr->trackList, a);
			return GF_OK;
		}
		{
			GF_TrackBox *tk = (GF_TrackBox *)a;
			//set our pointer to this obj
			tk->moov = ptr;
			tk->index = gf_list_count(ptr->trackList);
			if (tk->References) {
				GF_TrackReferenceTypeBox *dpnd=NULL;
				Track_FindRef(tk, GF_ISOM_REF_BASE, &dpnd);
				if (dpnd)
					tk->nb_base_refs = dpnd->trackIDCount;
			}
		}
		return gf_list_add(ptr->trackList, a);
	}
	return GF_OK;
}