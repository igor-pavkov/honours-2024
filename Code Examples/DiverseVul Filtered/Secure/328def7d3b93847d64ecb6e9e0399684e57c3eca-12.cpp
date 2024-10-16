GF_Err Media_FindDataRef(GF_DataReferenceBox *dref, char *URLname, char *URNname, u32 *dataRefIndex)
{
	u32 i;
	GF_DataEntryURLBox *entry;

	if (!dref) return GF_BAD_PARAM;
	*dataRefIndex = 0;
	i=0;
	while ((entry = (GF_DataEntryURLBox*)gf_list_enum(dref->child_boxes, &i))) {
		if (entry->type == GF_ISOM_BOX_TYPE_URL) {
			//self-contained case
			if (entry->flags == 1) {
				//if nothing specified, get the dataRef
				if (!URLname && !URNname) {
					*dataRefIndex = i;
					return GF_OK;
				}
			} else {
				//OK, check if we have URL
				if (URLname && !strcmp(URLname, entry->location)) {
					*dataRefIndex = i;
					return GF_OK;
				}
			}
		} else {
			//this is a URN one, only check the URN name (URL optional)
			if (URNname && !strcmp(URNname, ((GF_DataEntryURNBox *)entry)->nameURN)) {
				*dataRefIndex = i;
				return GF_OK;
			}
		}
	}
	return GF_OK;
}