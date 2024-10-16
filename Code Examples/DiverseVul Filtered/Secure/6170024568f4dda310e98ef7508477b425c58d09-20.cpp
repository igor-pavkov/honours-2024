void databox_box_del(GF_Box *s)
{
	GF_DataBox *ptr = (GF_DataBox *) s;
	if (ptr == NULL) return;
	if (ptr->data)
		gf_free(ptr->data);
	gf_free(ptr);

}