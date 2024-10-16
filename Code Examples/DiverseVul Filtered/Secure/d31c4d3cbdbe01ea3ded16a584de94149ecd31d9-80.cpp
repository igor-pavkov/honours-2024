static RBinObject *r_bin_object_new(RBinFile *binfile, RBinPlugin *plugin,
				     ut64 baseaddr, ut64 loadaddr, ut64 offset,
				     ut64 sz) {
	const ut8 *bytes = binfile? r_buf_buffer (binfile->buf): NULL;
	ut64 bytes_sz = binfile? r_buf_size (binfile->buf): 0;
	Sdb *sdb = binfile? binfile->sdb: NULL;
	RBinObject *o = R_NEW0 (RBinObject);
	if (!o) {
		return NULL;
	}
	o->obj_size = bytes && (bytes_sz >= sz + offset)? sz: 0;
	o->boffset = offset;
	o->id = r_num_rand (0xfffff000);
	o->kv = sdb_new0 ();
	o->baddr = baseaddr;
	o->baddr_shift = 0;
	o->plugin = plugin;
	o->loadaddr = loadaddr != UT64_MAX ? loadaddr : 0;

	// XXX more checking will be needed here
	// only use LoadBytes if buffer offset != 0
	// if (offset != 0 && bytes && plugin && plugin->load_bytes && (bytes_sz
	// >= sz + offset) ) {
	if (bytes && plugin && plugin->load_bytes && (bytes_sz >= sz + offset)) {
		ut64 bsz = bytes_sz - offset;
		if (sz < bsz) {
			bsz = sz;
		}
		o->bin_obj = plugin->load_bytes (binfile, bytes + offset, sz,
						 loadaddr, sdb);
		if (!o->bin_obj) {
			bprintf (
				"Error in r_bin_object_new: load_bytes failed "
				"for %s plugin\n",
				plugin->name);
			sdb_free (o->kv);
			free (o);
			return NULL;
		}
	} else if (binfile && plugin && plugin->load) {
		// XXX - haha, this is a hack.
		// switching out the current object for the new
		// one to be processed
		RBinObject *old_o = binfile->o;
		binfile->o = o;
		if (plugin->load (binfile)) {
			binfile->sdb_info = o->kv;
			// mark as do not walk
			sdb_ns_set (binfile->sdb, "info", o->kv);
		} else {
			binfile->o = old_o;
		}
		o->obj_size = sz;
	} else {
		sdb_free (o->kv);
		free (o);
		return NULL;
	}

	// XXX - binfile could be null here meaning an improper load
	// XXX - object size cant be set here and needs to be set where
	// where the object is created from.  The reason for this is to prevent
	// mis-reporting when the file is loaded from impartial bytes or is
	// extracted
	// from a set of bytes in the file
	r_bin_object_set_items (binfile, o);
	r_bin_file_object_add (binfile, o);

	// XXX this is a very hacky alternative to rewriting the
	// RIO stuff, as discussed here:
	return o;
}