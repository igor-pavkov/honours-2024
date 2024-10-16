GF_UserDataMap *udta_getEntry(GF_UserDataBox *ptr, u32 box_type, bin128 *uuid)
{
	u32 i;
	GF_UserDataMap *map;
	if (ptr == NULL) return NULL;
	i=0;
	while ((map = (GF_UserDataMap *)gf_list_enum(ptr->recordList, &i))) {
		if (map->boxType == box_type) {
			if ((box_type != GF_ISOM_BOX_TYPE_UUID) || !uuid) return map;
			if (!memcmp(map->uuid, *uuid, 16)) return map;
		}
	}
	return NULL;
}