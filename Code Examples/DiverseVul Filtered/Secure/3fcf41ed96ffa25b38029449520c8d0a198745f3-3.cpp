R_API void r_bin_file_free(void /*RBinFile*/ *bf_) {
	RBinFile *a = bf_;
	RBinPlugin *plugin = r_bin_file_cur_plugin (a);

	if (!a) {
		return;
	}
	// Binary format objects are connected to the
	// RBinObject, so the plugin must destroy the
	// format data first
	if (plugin && plugin->destroy) {
		plugin->destroy (a);
	}
	if (a->curxtr && a->curxtr->destroy && a->xtr_obj) {
		a->curxtr->free_xtr ((void *)(a->xtr_obj));
	}
	r_buf_free (a->buf);
	// TODO: unset related sdb namespaces
	if (a && a->sdb_addrinfo) {
		sdb_free (a->sdb_addrinfo);
		a->sdb_addrinfo = NULL;
	}
	R_FREE (a->file);
	a->o = NULL;
	r_list_free (a->objs);
	r_list_free (a->xtr_data);
	if (a->id != -1) {
		r_id_pool_kick_id (a->rbin->file_ids, a->id);
	}
	memset (a, 0, sizeof (RBinFile));
	free (a);
}