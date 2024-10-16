R_API bool r_bin_file_object_new_from_xtr_data(RBin *bin, RBinFile *bf,
						ut64 baseaddr, ut64 loadaddr,
						RBinXtrData *data) {
	RBinObject *o = NULL;
	RBinPlugin *plugin = NULL;
	ut8* bytes;
	ut64 offset = data? data->offset: 0;
	ut64 sz = data ? data->size : 0;
	if (!data || !bf) {
		return false;
	}

	// for right now the bytes used will just be the offest into the binfile
	// buffer
	// if the extraction requires some sort of transformation then this will
	// need to be fixed
	// here.
	bytes = data->buffer;
	if (!bytes) {
		return false;
	}
	plugin = r_bin_get_binplugin_by_bytes (bin, (const ut8*)bytes, sz);
	if (!plugin) {
		plugin = r_bin_get_binplugin_any (bin);
	}
	r_buf_free (bf->buf);
	bf->buf = r_buf_new_with_bytes ((const ut8*)bytes, data->size);
	//r_bin_object_new append the new object into binfile
	o = r_bin_object_new (bf, plugin, baseaddr, loadaddr, offset, sz);
	// size is set here because the reported size of the object depends on
	// if loaded from xtr plugin or partially read
	if (!o) {
		return false;
	}
	if (o && !o->size) {
		o->size = sz;
	}
	bf->narch = data->file_count;
	if (!o->info) {
		o->info = R_NEW0 (RBinInfo);
	}
	free (o->info->file);
	free (o->info->arch);
	free (o->info->machine);
	free (o->info->type);
	o->info->file = strdup (bf->file);
	o->info->arch = strdup (data->metadata->arch);
	o->info->machine = strdup (data->metadata->machine);
	o->info->type = strdup (data->metadata->type);
	o->info->bits = data->metadata->bits;
	o->info->has_crypto = bf->o->info->has_crypto;
	data->loaded = true;
	return true;
}