void fdpa_box_del(GF_Box *s)
{
	u32 i;
	GF_FDpacketBox *ptr = (GF_FDpacketBox *)s;
	if (ptr == NULL) return;

	if (ptr->headers) {
		for (i=0; i<ptr->header_ext_count; i++) {
			if (ptr->headers[i].data) gf_free(ptr->headers[i].data);
		}
		gf_free(ptr->headers);
	}
	gf_free(ptr);
}