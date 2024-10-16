void piff_pssh_box_del(GF_Box *s)
{
	GF_PIFFProtectionSystemHeaderBox *ptr = (GF_PIFFProtectionSystemHeaderBox*)s;
	if (ptr->private_data) gf_free(ptr->private_data);
	gf_free(s);
}