void ilst_box_del(GF_Box *s)
{
	GF_ItemListBox *ptr = (GF_ItemListBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}