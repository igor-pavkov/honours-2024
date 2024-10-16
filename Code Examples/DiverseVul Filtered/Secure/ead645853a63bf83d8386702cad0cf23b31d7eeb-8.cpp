static RList *dex_method_signature2(RBinDexObj *bin, int method_idx) {
	ut32 proto_id, params_off, list_size;
	char *buff = NULL; 
	ut8 *bufptr;
	ut16 type_idx;
	int i;

	RList *params = r_list_newf (free);
	if (!params) {
		return NULL;
	}
	if (method_idx < 0 || method_idx >= bin->header.method_size) {
		goto out_error;
	}
	proto_id = bin->methods[method_idx].proto_id;
	if (proto_id >= bin->header.prototypes_size) {
		goto out_error;
	}
	params_off = bin->protos[proto_id].parameters_off;
	if (params_off  >= bin->size) {
		goto out_error;
	}
	if (!params_off) {
		return params;
	}
	bufptr = bin->b->buf;
	// size of the list, in entries
	list_size = r_read_le32 (bufptr + params_off); 
	//XXX list_size tainted it may produce huge loop
	for (i = 0; i < list_size; i++) {
		ut64 of = params_off + 4 + (i * 2);
		if (of >= bin->size || of < params_off) {
			break;
		}
		type_idx = r_read_le16 (bufptr + of);
		if (type_idx >= bin->header.types_size ||
		    type_idx > bin->size) {
			break;
		}
		buff = getstr (bin, bin->types[type_idx].descriptor_id);
		if (!buff) {
			break;
		}
		r_list_append (params, buff);
	}
	return params;
out_error:
	r_list_free (params);
	return NULL;
}