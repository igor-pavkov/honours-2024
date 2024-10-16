GF_Err piff_tenc_box_size(GF_Box *s)
{
	GF_PIFFTrackEncryptionBox *ptr = (GF_PIFFTrackEncryptionBox*)s;
	ptr->size += 24;
	return GF_OK;
}