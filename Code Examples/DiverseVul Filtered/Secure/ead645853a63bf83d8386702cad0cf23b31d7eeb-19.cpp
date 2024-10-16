static char *dex_method_signature(RBinDexObj *bin, int method_idx) {
	ut32 proto_id, params_off, type_id, list_size;
	char *r, *return_type = NULL, *signature = NULL, *buff = NULL; 
	ut8 *bufptr;
	ut16 type_idx;
	int pos = 0, i, size = 1;

	if (method_idx < 0 || method_idx >= bin->header.method_size) {
		return NULL;
	}
	proto_id = bin->methods[method_idx].proto_id;
	if (proto_id >= bin->header.prototypes_size) {
		return NULL;
	}
	params_off = bin->protos[proto_id].parameters_off;
	if (params_off  >= bin->size) {
		return NULL;
	}
	type_id = bin->protos[proto_id].return_type_id;
	if (type_id >= bin->header.types_size ) {
		return NULL;
	}
	return_type = getstr (bin, bin->types[type_id].descriptor_id);
	if (!return_type) {
		return NULL;
	}
	if (!params_off) {
		return r_str_newf ("()%s", return_type);;
	}
	bufptr = bin->b->buf;
	// size of the list, in entries
	list_size = r_read_le32 (bufptr + params_off); 
	//XXX again list_size is user controlled huge loop
	for (i = 0; i < list_size; i++) {
		int buff_len = 0;
		if (params_off + 4 + (i * 2) >= bin->size) {
			break;
		}
		type_idx = r_read_le16 (bufptr + params_off + 4 + (i * 2));
		if (type_idx < 0 ||
		    type_idx >=
			    bin->header.types_size || type_idx >= bin->size) {
			break;
		}
		buff = getstr (bin, bin->types[type_idx].descriptor_id);
		if (!buff) {
			break;
		}
		buff_len = strlen (buff);
		size += buff_len + 1;
		signature = realloc (signature, size);
		strcpy (signature + pos, buff);
		pos += buff_len;
		signature[pos] = '\0';
	}
	r = r_str_newf ("(%s)%s", signature, return_type);
	free (buff);
	free (signature);
	return r;
}