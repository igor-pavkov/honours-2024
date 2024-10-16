GF_Err schm_box_size(GF_Box *s)
{
	GF_SchemeTypeBox *ptr = (GF_SchemeTypeBox *) s;
	if (!s) return GF_BAD_PARAM;
	ptr->size += 8;
	if (ptr->flags & 0x000001) ptr->size += 1 + (ptr->URI ? strlen(ptr->URI) : 0);
	return GF_OK;
}