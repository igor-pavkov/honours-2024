void cprt_box_del(GF_Box *s)
{
	GF_CopyrightBox *ptr = (GF_CopyrightBox *) s;
	if (ptr == NULL) return;
	if (ptr->notice)
		gf_free(ptr->notice);
	gf_free(ptr);
}