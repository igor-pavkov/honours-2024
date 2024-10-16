GF_Err Media_CreateDataRef(GF_ISOFile *movie, GF_DataReferenceBox *dref, char *URLname, char *URNname, u32 *dataRefIndex)
{
	GF_Err e;
	Bool use_alis=GF_FALSE;
	GF_DataEntryURLBox *entry;

	if (URLname && !strcmp(URLname, "alis")) {
		URLname = NULL;
		use_alis=GF_TRUE;
	}

	if (!URLname && !URNname) {
		//THIS IS SELF CONTAIN, create a regular entry if needed
		entry = (GF_DataEntryURLBox *) gf_isom_box_new_parent(&dref->child_boxes, use_alis ? GF_QT_BOX_TYPE_ALIS : GF_ISOM_BOX_TYPE_URL);
		if (!entry) return GF_OUT_OF_MEM;
		entry->flags = 1;
		*dataRefIndex = gf_list_count(dref->child_boxes);
		return GF_OK;
	} else if (!URNname && URLname) {
		//THIS IS URL
		entry = (GF_DataEntryURLBox *) gf_isom_box_new_parent(&dref->child_boxes, GF_ISOM_BOX_TYPE_URL);
		if (!entry) return GF_OUT_OF_MEM;
		entry->flags = 0;

		e = Media_SetDrefURL(entry, URLname, movie->fileName ? movie->fileName : movie->finalName);
		if (! entry->location) {
			gf_isom_box_del_parent(&dref->child_boxes, (GF_Box *)entry);
			return e ? e : GF_OUT_OF_MEM;
		}
		*dataRefIndex = gf_list_count(dref->child_boxes);
		return GF_OK;
	} else {
		//THIS IS URN
		entry = (GF_DataEntryURLBox *) gf_isom_box_new_parent(&dref->child_boxes, GF_ISOM_BOX_TYPE_URN);
		if (!entry) return GF_OUT_OF_MEM;
		((GF_DataEntryURNBox *)entry)->flags = 0;
		((GF_DataEntryURNBox *)entry)->nameURN = (char*)gf_malloc(strlen(URNname)+1);
		if (! ((GF_DataEntryURNBox *)entry)->nameURN) {
			gf_isom_box_del_parent(&dref->child_boxes, (GF_Box *)entry);
			return GF_OUT_OF_MEM;
		}
		strcpy(((GF_DataEntryURNBox *)entry)->nameURN, URNname);
		//check for URL
		if (URLname) {
			((GF_DataEntryURNBox *)entry)->location = (char*)gf_malloc(strlen(URLname)+1);
			if (! ((GF_DataEntryURNBox *)entry)->location) {
				gf_isom_box_del_parent(&dref->child_boxes, (GF_Box *)entry);
				return GF_OUT_OF_MEM;
			}
			strcpy(((GF_DataEntryURNBox *)entry)->location, URLname);
		}
		*dataRefIndex = gf_list_count(dref->child_boxes);
		return GF_OK;
	}
	return GF_OK;
}