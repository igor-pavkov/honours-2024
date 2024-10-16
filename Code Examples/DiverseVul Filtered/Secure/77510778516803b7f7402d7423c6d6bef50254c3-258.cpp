void dfla_box_del(GF_Box *s)
{
	GF_FLACConfigBox *ptr = (GF_FLACConfigBox *) s;
	if (ptr->data) gf_free(ptr->data);
	gf_free(ptr);
}