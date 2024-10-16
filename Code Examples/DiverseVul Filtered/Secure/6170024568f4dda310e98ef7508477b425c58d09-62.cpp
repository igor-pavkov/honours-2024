GF_Err databox_box_size(GF_Box *s)
{
	GF_DataBox *ptr = (GF_DataBox *)s;

	ptr->size += 4;
	if(ptr->data != NULL && ptr->dataSize > 0) {
		ptr->size += ptr->dataSize;
	}
	return GF_OK;
}