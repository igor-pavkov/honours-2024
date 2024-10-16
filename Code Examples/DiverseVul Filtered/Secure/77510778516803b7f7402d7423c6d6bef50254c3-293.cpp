void fdsa_box_del(GF_Box *s)
{
	GF_HintSample *ptr = (GF_HintSample *)s;
	gf_list_del(ptr->packetTable);
	gf_free(ptr);
}