void leva_box_del(GF_Box *s)
{
	GF_LevelAssignmentBox *ptr = (GF_LevelAssignmentBox *)s;
	if (ptr == NULL) return;
	if (ptr->levels) gf_free(ptr->levels);
	gf_free(ptr);
}