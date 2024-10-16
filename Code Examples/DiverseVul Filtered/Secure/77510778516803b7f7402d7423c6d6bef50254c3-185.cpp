GF_Err fdpa_box_size(GF_Box *s)
{
	u32 i;
	GF_FDpacketBox *ptr = (GF_FDpacketBox *)s;

	ptr->size += 5;

	for (i=0; i<ptr->header_ext_count; i++) {
		ptr->size += 1;
		if (ptr->headers[i].header_extension_type > 127) {
			ptr->size += 3;
		} else {
			ptr->size += 1 + ptr->headers[i].data_length;
		}
	}
	return GF_OK;
}