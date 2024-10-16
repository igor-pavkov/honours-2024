void ilst_item_box_del(GF_Box *s)
{
	GF_ListItemBox *ptr = (GF_ListItemBox *) s;
	if (ptr == NULL) return;
	gf_free(ptr);
}