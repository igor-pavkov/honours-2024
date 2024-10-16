void stdp_box_del(GF_Box *s)
{
	GF_DegradationPriorityBox *ptr = (GF_DegradationPriorityBox *)s;
	if (ptr == NULL ) return;
	if (ptr->priorities) gf_free(ptr->priorities);
	gf_free(ptr);
}