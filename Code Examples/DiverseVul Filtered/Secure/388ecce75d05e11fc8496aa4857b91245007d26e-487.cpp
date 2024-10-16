void hdlr_box_del(GF_Box *s)
{
	GF_HandlerBox *ptr = (GF_HandlerBox *)s;
	if (ptr == NULL) return;
	if (ptr->nameUTF8) gf_free(ptr->nameUTF8);
	gf_free(ptr);
}