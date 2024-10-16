GF_Err mdia_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_MediaBox *ptr = (GF_MediaBox *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_MDHD:
		if (ptr->mediaHeader) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->mediaHeader = (GF_MediaHeaderBox *)a;
		return GF_OK;

	case GF_ISOM_BOX_TYPE_HDLR:
		if (ptr->handler) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->handler = (GF_HandlerBox *)a;
		return GF_OK;

	case GF_ISOM_BOX_TYPE_MINF:
		if (ptr->information) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->information = (GF_MediaInformationBox *)a;
		return GF_OK;
	}
	return GF_OK;
}