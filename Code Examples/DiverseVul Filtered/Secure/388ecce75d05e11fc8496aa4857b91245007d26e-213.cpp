void free_box_del(GF_Box *s)
{
	GF_FreeSpaceBox *ptr = (GF_FreeSpaceBox *)s;
	if (ptr->data) gf_free(ptr->data);
	gf_free(ptr);
}