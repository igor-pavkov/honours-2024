GF_ItemListBox *gf_isom_locate_box(GF_List *list, u32 boxType, bin128 UUID)
{
	u32 i;
	GF_Box *box;
	i=0;
	while ((box = (GF_Box *)gf_list_enum(list, &i))) {
		if (box->type == boxType) {
			GF_UUIDBox* box2 = (GF_UUIDBox* )box;
			if (boxType != GF_ISOM_BOX_TYPE_UUID) return (GF_ItemListBox *)box;
			if (!memcmp(box2->uuid, UUID, 16)) return (GF_ItemListBox *)box;
		}
	}
	return NULL;
}