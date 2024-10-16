GF_Err udta_box_size(GF_Box *s)
{
	GF_Err e;
	u32 i;
	GF_UserDataMap *map;
	GF_UserDataBox *ptr = (GF_UserDataBox *)s;

	i=0;
	while ((map = (GF_UserDataMap *)gf_list_enum(ptr->recordList, &i))) {
		//warning: here we are not passing the actual "parent" of the list
		//but the UDTA box. The parent itself is not an box, we don't care about it
		e = gf_isom_box_array_size(s, map->boxes);
		if (e) return e;
	}
	return GF_OK;
}