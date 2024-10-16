GF_Err tenc_box_size(GF_Box *s)
{
	GF_TrackEncryptionBox *ptr = (GF_TrackEncryptionBox*)s;
	ptr->size += 3;

	ptr->size += 17;
	if ((ptr->isProtected == 1) && ! ptr->key_info[3]) {
		ptr->size += 1 + ptr->key_info[20];
	}
	return GF_OK;
}