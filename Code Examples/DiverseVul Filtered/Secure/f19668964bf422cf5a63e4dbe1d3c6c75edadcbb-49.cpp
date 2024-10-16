GF_Err pssh_box_size(GF_Box *s)
{
	GF_ProtectionSystemHeaderBox *ptr = (GF_ProtectionSystemHeaderBox*)s;

	if (ptr->KID_count && !ptr->version) {
		ptr->version = 1;
	}

	ptr->size += 16;
	if (ptr->version) ptr->size += 4 + 16*ptr->KID_count;
	ptr->size += 4 + (ptr->private_data ? ptr->private_data_size : 0);
	return GF_OK;
}