GF_Err iKMS_box_size(GF_Box *s)
{
	GF_ISMAKMSBox *ptr = (GF_ISMAKMSBox *)s;
    ptr->size += (ptr->URI ? strlen(ptr->URI) : 0) + 1;
	return GF_OK;
}