GF_Err piff_pssh_box_size(GF_Box *s)
{
	GF_PIFFProtectionSystemHeaderBox *ptr = (GF_PIFFProtectionSystemHeaderBox*)s;

	ptr->size += 24 + ptr->private_data_size;
	return GF_OK;
}