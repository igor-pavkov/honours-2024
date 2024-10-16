R_API int r_bin_object_set_items(RBinFile *binfile, RBinObject *o) {
	RBinObject *old_o;
	RBinPlugin *cp;
	int i, minlen;
	RBin *bin;
	if (!binfile || !o || !o->plugin) {
		return false;
	}
	bin = binfile->rbin;
	old_o = binfile->o;
	cp = o->plugin;
	if (binfile->rbin->minstrlen > 0) {
		minlen = binfile->rbin->minstrlen;
	} else {
		minlen = cp->minstrlen;
	}
	binfile->o = o;
	if (cp->baddr) {
		ut64 old_baddr = o->baddr;
		o->baddr = cp->baddr (binfile);
		binobj_set_baddr (o, old_baddr);
	}
	if (cp->boffset) {
		o->boffset = cp->boffset (binfile);
	}
	// XXX: no way to get info from xtr pluginz?
	// Note, object size can not be set from here due to potential
	// inconsistencies
	if (cp->size) {
		o->size = cp->size (binfile);
	}
	if (cp->binsym) {
		for (i = 0; i < R_BIN_SYM_LAST; i++) {
			o->binsym[i] = cp->binsym (binfile, i);
			if (o->binsym[i]) {
				o->binsym[i]->paddr += o->loadaddr;
			}
		}
	}
	if (cp->entries) {
		o->entries = cp->entries (binfile);
		REBASE_PADDR (o, o->entries, RBinAddr);
	}
	if (cp->fields) {
		o->fields = cp->fields (binfile);
		if (o->fields) {
			o->fields->free = r_bin_field_free;
			REBASE_PADDR (o, o->fields, RBinField);
		}
	}
	if (cp->imports) {
		r_list_free (o->imports);
		o->imports = cp->imports (binfile);
		if (o->imports) {
			o->imports->free = r_bin_import_free;
		}
	}
	//if (bin->filter_rules & (R_BIN_REQ_SYMBOLS | R_BIN_REQ_IMPORTS)) {
	if (true) {
		if (cp->symbols) {
			o->symbols = cp->symbols (binfile);
			if (o->symbols) {
				o->symbols->free = r_bin_symbol_free;
				REBASE_PADDR (o, o->symbols, RBinSymbol);
				if (bin->filter) {
					r_bin_filter_symbols (o->symbols);
				}
			}
		}
	}
	//}
	o->info = cp->info? cp->info (binfile): NULL;
	if (cp->libs) {
		o->libs = cp->libs (binfile);
	}
	if (cp->sections) {
		// XXX sections are populated by call to size
		if (!o->sections) {
			o->sections = cp->sections (binfile);
		}
		REBASE_PADDR (o, o->sections, RBinSection);
		if (bin->filter) {
			r_bin_filter_sections (o->sections);
		}
	}
	if (bin->filter_rules & (R_BIN_REQ_RELOCS | R_BIN_REQ_IMPORTS)) {
		if (cp->relocs) {
			o->relocs = cp->relocs (binfile);
			REBASE_PADDR (o, o->relocs, RBinReloc);
		}
	}
	if (bin->filter_rules & R_BIN_REQ_STRINGS) {
		if (cp->strings) {
			o->strings = cp->strings (binfile);
		} else {
			o->strings = get_strings (binfile, minlen, 0);
		}
		if (bin->debase64) {
			filterStrings (bin, o->strings);
		}
		REBASE_PADDR (o, o->strings, RBinString);
	}
	if (bin->filter_rules & R_BIN_REQ_CLASSES) {
		if (cp->classes) {
			o->classes = cp->classes (binfile);
			if (r_bin_lang_swift (binfile)) {
				o->classes = r_bin_classes_from_symbols (binfile, o);
			}
		} else {
			o->classes = r_bin_classes_from_symbols (binfile, o);
		}
		if (bin->filter) {
			r_bin_filter_classes (o->classes);
		}
	}
	if (cp->lines) {
		o->lines = cp->lines (binfile);
	}
	if (cp->get_sdb) {
		Sdb* new_kv = cp->get_sdb (binfile);
		if (new_kv != o->kv) {
			sdb_free (o->kv);
		}
		o->kv = new_kv;
	}
	if (cp->mem)  {
		o->mem = cp->mem (binfile);
	}
	if (bin->filter_rules & (R_BIN_REQ_SYMBOLS | R_BIN_REQ_IMPORTS)) {
		o->lang = r_bin_load_languages (binfile);
	}
	binfile->o = old_o;
	return true;
}