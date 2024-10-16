GF_Err udta_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_UserDataMap *map;
	GF_UserDataBox *ptr = (GF_UserDataBox *)s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	i=0;
	while ((map = (GF_UserDataMap *)gf_list_enum(ptr->recordList, &i))) {
		//warning: here we are not passing the actual "parent" of the list
		//but the UDTA box. The parent itself is not an box, we don't care about it
		e = gf_isom_box_array_write(s, map->boxes, bs);
		if (e) return e;
	}
	return GF_OK;
}