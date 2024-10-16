R_API bool r_bin_file_object_new_from_xtr_data(RBin *bin, RBinFile *bf, ut64 baseaddr, ut64 loadaddr, RBinXtrData *data) {
	r_return_val_if_fail (bin && bf && data, false);

	ut64 offset = data->offset;
	ut64 sz = data->size;

	RBinPlugin *plugin = get_plugin_from_buffer (bin, bf, NULL, data->buf);
	bf->buf = r_buf_ref (data->buf);

	RBinObject *o = r_bin_object_new (bf, plugin, baseaddr, loadaddr, offset, sz);
	if (!o) {
		return false;
	}
	// size is set here because the reported size of the object depends on
	// if loaded from xtr plugin or partially read
	if (!o->size) {
		o->size = sz;
	}
	bf->narch = data->file_count;
	if (!o->info) {
		o->info = R_NEW0 (RBinInfo);
	}
	R_FREE (o->info->file);
	R_FREE (o->info->arch);
	R_FREE (o->info->machine);
	R_FREE (o->info->type);
	o->info->file = strdup (bf->file);
	if (data->metadata) {
		if (data->metadata->arch) {
			o->info->arch = strdup (data->metadata->arch);
		}
		if (data->metadata->machine) {
			o->info->machine = strdup (data->metadata->machine);
		}
		if (data->metadata->type) {
			o->info->type = strdup (data->metadata->type);
		}
		o->info->bits = data->metadata->bits;
	}
	o->info->has_crypto = bf->o->info->has_crypto;
	data->loaded = true;
	return true;
}