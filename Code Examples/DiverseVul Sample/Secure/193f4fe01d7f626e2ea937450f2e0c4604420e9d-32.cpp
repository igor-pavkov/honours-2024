static void get_strings_range(RBinFile *bf, RList *list, int min, int raw, ut64 from, ut64 to, RBinSection *section) {
	r_return_if_fail (bf && bf->buf);

	RBinPlugin *plugin = r_bin_file_cur_plugin (bf);

	if (!raw && (!plugin || !plugin->info)) {
		return;
	}
	if (!min) {
		min = plugin? plugin->minstrlen: 4;
	}
	/* Some plugins return zero, fix it up */
	if (min < 0) {
		return;
	}
	if (!min) {
		min = 4;
	}
	{
		RIO *io = bf->rbin->iob.io;
		RCoreBind *cb = &io->coreb;
		if (cb && cb->cfgGet) {
			const bool cfg_debug = cb->cfgGet (cb->core, "cfg.debug");
			if (!cfg_debug) {
				if (!to || to > r_buf_size (bf->buf)) {
					to = r_buf_size (bf->buf);
				}
				if (!to) {
					return;
				}
			}
		}
	}
	if (raw != 2) {
		ut64 size = to - from;
		// in case of dump ignore here
		if (bf->rbin->maxstrbuf && size && size > bf->rbin->maxstrbuf) {
			if (bf->rbin->verbose) {
				R_LOG_WARN ("bin_strings buffer is too big (0x%08" PFMT64x "). Use -zzz or set bin.maxstrbuf (RABIN2_MAXSTRBUF) in r2 (rabin2)", size);
			}
			return;
		}
	}
	int type;
	const char *enc = bf->rbin->strenc;
	if (!enc) {
		type = R_STRING_TYPE_DETECT;
	} else if (!strcmp (enc, "latin1")) {
		type = R_STRING_TYPE_ASCII;
	} else if (!strcmp (enc, "utf8")) {
		type = R_STRING_TYPE_UTF8;
	} else if (!strcmp (enc, "utf16le")) {
		type = R_STRING_TYPE_WIDE;
	} else if (!strcmp (enc, "utf32le")) {
		type = R_STRING_TYPE_WIDE32;
	} else { // TODO utf16be, utf32be
		eprintf ("ERROR: encoding %s not supported\n", enc);
		return;
	}
	string_scan_range (list, bf, min, from, to, type, raw, section);
}