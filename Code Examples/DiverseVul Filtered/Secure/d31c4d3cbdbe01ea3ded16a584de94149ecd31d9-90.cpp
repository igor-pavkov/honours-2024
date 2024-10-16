R_API RBinXtrData *r_bin_xtrdata_new(RBuffer *buf, ut64 offset, ut64 size,
				      ut32 file_count,
				      RBinXtrMetadata *metadata) {
	RBinXtrData *data = R_NEW0 (RBinXtrData);
	if (!data) {
		return NULL;
	}
	data->offset = offset;
	data->size = size;
	data->file_count = file_count;
	data->metadata = metadata;
	data->loaded = 0;
	data->buffer = malloc (size + 1);
	// data->laddr = 0; /// XXX
	if (!data->buffer) {
		free (data);
		return NULL;
	}
	memcpy (data->buffer, r_buf_buffer (buf), size);
	data->buffer[size] = 0;
	return data;
}