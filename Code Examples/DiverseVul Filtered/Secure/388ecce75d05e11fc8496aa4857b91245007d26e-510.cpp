void name_box_del(GF_Box *s)
{
	GF_NameBox *name = (GF_NameBox *)s;
	if (name->string) gf_free(name->string);
	gf_free(name);
}