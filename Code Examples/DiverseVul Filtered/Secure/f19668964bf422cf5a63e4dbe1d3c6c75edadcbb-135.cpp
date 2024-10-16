void pssh_box_del(GF_Box *s)
{
	GF_ProtectionSystemHeaderBox *ptr = (GF_ProtectionSystemHeaderBox*)s;
	if (ptr == NULL) return;
	if (ptr->private_data) gf_free(ptr->private_data);
	if (ptr->KIDs) gf_free(ptr->KIDs);
	gf_free(ptr);
}