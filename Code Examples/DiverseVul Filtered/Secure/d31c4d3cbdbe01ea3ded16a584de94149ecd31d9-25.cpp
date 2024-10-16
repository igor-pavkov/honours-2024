R_API RBuffer *r_bin_create(RBin *bin, const ut8 *code, int codelen,
			     const ut8 *data, int datalen) {
	RBinFile *a = r_bin_cur (bin);
	RBinPlugin *plugin = r_bin_file_cur_plugin (a);
	if (codelen < 0) {
		codelen = 0;
	}
	if (datalen < 0) {
		datalen = 0;
	}
	if (plugin && plugin->create) {
		return plugin->create (bin, code, codelen, data, datalen);
	}
	return NULL;
}