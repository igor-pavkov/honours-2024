void udta_box_del(GF_Box *s)
{
	u32 i;
	GF_UserDataMap *map;
	GF_UserDataBox *ptr = (GF_UserDataBox *)s;
	if (ptr == NULL) return;
	i=0;
	while ((map = (GF_UserDataMap *)gf_list_enum(ptr->recordList, &i))) {
		gf_isom_box_array_del(map->boxes);
		gf_free(map);
	}
	gf_list_del(ptr->recordList);
	gf_free(ptr);
}