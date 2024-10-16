GF_Err hdlr_box_size(GF_Box *s)
{
	GF_HandlerBox *ptr = (GF_HandlerBox *)s;
	ptr->size += 20 + 1; //null term or counted string
	if (ptr->nameUTF8) {
		ptr->size += strlen(ptr->nameUTF8);
	}
	return GF_OK;
}