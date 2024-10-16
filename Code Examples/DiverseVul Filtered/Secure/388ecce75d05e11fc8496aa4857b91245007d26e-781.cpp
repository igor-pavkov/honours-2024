GF_Err kind_box_size(GF_Box *s)
{
	GF_KindBox *ptr = (GF_KindBox *)s;

    ptr->size += (ptr->schemeURI ? strlen(ptr->schemeURI) : 0) + 1;
	if (ptr->value) {
		ptr->size += strlen(ptr->value) + 1;
	}
	return GF_OK;
}