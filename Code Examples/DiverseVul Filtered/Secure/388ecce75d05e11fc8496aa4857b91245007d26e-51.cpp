GF_Err kind_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_KindBox *ptr = (GF_KindBox *)s;

	if (ptr->size) {
		u32 bytesToRead = (u32) ptr->size;
		char *data;
		u32 schemeURIlen;
		data = (char*)gf_malloc(bytesToRead * sizeof(char));
		if (!data) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, data, bytesToRead);
		/*safety check in case the string is not null-terminated*/
		if (data[bytesToRead-1]) {
			data = (char*)gf_realloc(data, sizeof(char)*(bytesToRead + 1));
			if (!data) return GF_OUT_OF_MEM;
			data[bytesToRead] = 0;
			bytesToRead++;
		}
		ptr->schemeURI = gf_strdup(data);
		if (!ptr->schemeURI) return GF_OUT_OF_MEM;
		schemeURIlen = (u32) strlen(data);
		if (bytesToRead > schemeURIlen+1) {
			/* read the value */
			char *data_value = data + schemeURIlen +1;
			ptr->value = gf_strdup(data_value);
			if (!ptr->value) return GF_OUT_OF_MEM;
		}
		gf_free(data);
	}
	return GF_OK;
}