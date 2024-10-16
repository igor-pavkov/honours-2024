static u32 do_add_cat(int argc, char **argv)
{
	GF_Err e;
	u32 i, ipass, nb_pass = 1;
	char *mux_args=NULL;
	GF_FilterSession *fs = NULL;
	if (nb_add) {

		GF_ISOOpenMode open_mode = GF_ISOM_OPEN_EDIT;
		if (force_new) {
			open_mode = (do_flat || (force_new==2)) ? GF_ISOM_OPEN_WRITE : GF_ISOM_WRITE_EDIT;
		} else {
			FILE *test = gf_fopen(inName, "rb");
			if (!test) {
				open_mode = (do_flat) ? GF_ISOM_OPEN_WRITE : GF_ISOM_WRITE_EDIT;
				if (!outName) outName = inName;
			} else {
				gf_fclose(test);
				if (! gf_isom_probe_file(inName) ) {
					open_mode = (do_flat) ? GF_ISOM_OPEN_WRITE : GF_ISOM_WRITE_EDIT;
					if (!outName) outName = inName;
				}
			}
		}
		open_edit = do_flat ? GF_FALSE : GF_TRUE;
		file = gf_isom_open(inName, open_mode, NULL);
		if (!file) {
			M4_LOG(GF_LOG_ERROR, ("Cannot open destination file %s: %s\n", inName, gf_error_to_string(gf_isom_last_error(NULL)) ));
			return mp4box_cleanup(1);
		}

		if (freeze_box_order)
			gf_isom_freeze_order(file);

		if (do_flat) {
			if (major_brand)
				gf_isom_set_brand_info(file, major_brand, minor_version);
			for (i=0; i<nb_alt_brand_add; i++) {
				gf_isom_modify_alternate_brand(file, brand_add[i], GF_TRUE);
				do_save = GF_TRUE;
			}

			if (!major_brand && !nb_alt_brand_add && has_add_image) {
				gf_isom_modify_alternate_brand(file, GF_ISOM_BRAND_MIF1, GF_TRUE);
			}
		}
	}

	if (file && keep_utc && open_edit) {
		gf_isom_keep_utc_times(file, 1);
	}

	if (do_flat && interleaving_time) {
		char szSubArg[100];
		gf_isom_set_storage_mode(file, GF_ISOM_STORE_FASTSTART);
		do_flat = 2;
		nb_pass = 2;
		fs = gf_fs_new_defaults(0);
		if (!fs) {
			M4_LOG(GF_LOG_ERROR, ("Error creating filter session\n"));
			gf_isom_delete(file);
			return mp4box_cleanup(1);
		}

		//mux args
		gf_dynstrcat(&mux_args, "mp4mx:importer:store=fstart", ":");

		sprintf(szSubArg, "file=%p", file);
		gf_dynstrcat(&mux_args, szSubArg, ":");
		sprintf(szSubArg, "cdur=%g", interleaving_time);
		gf_dynstrcat(&mux_args, szSubArg, ":");
	}

	for (ipass=0; ipass<nb_pass; ipass++) {
		u32 tk_idx = 1;
		for (i=0; i<(u32) argc; i++) {
			char *margs=NULL;
			if (!strcmp(argv[i], "-add")) {
				char *src = argv[i+1];

				while (src) {
					char *loc_src = src;
					char *sep = NULL;
					while (1) {
						char *opt_sep;
						sep = strchr(loc_src, '+');
						if (!sep) break;

						sep[0] = 0;
						if (strstr(src, "://"))
							break;

						opt_sep = gf_url_colon_suffix(src);
						if (opt_sep)
							opt_sep[0] = 0;
						if (gf_file_exists(src)) {
							if (opt_sep)
								opt_sep[0] = ':';
							break;
						}
						if (opt_sep)
							opt_sep[0] = ':';

						sep[0] = '+';
						loc_src = sep+1;
					}

					e = import_file(file, src, import_flags, import_fps, agg_samples, fs, (fs && (ipass==0)) ? &margs : NULL, tk_idx);
					tk_idx++;

					if (margs) {
						gf_dynstrcat(&mux_args, margs, ":");
						gf_free(margs);
					}

					if (e) {
						M4_LOG(GF_LOG_ERROR, ("Error importing %s: %s\n", argv[i+1], gf_error_to_string(e)));
						gf_isom_delete(file);
						if (fs)
							gf_fs_del(fs);
						return mp4box_cleanup(1);
					}
					if (sep) {
						sep[0] = '+';
						src = sep+1;
					} else {
						break;
					}
				}
				i++;
			} else if (!strcmp(argv[i], "-cat") || !strcmp(argv[i], "-catx") || !strcmp(argv[i], "-catpl")) {
				if (nb_pass == 2) {
					M4_LOG(GF_LOG_ERROR, ("Cannot cat files when using -newfs mode\n"));
					return mp4box_cleanup(1);
				}
				if (!file) {
					u8 open_mode = GF_ISOM_OPEN_EDIT;
					if (force_new) {
						open_mode = (do_flat) ? GF_ISOM_OPEN_WRITE : GF_ISOM_WRITE_EDIT;
					} else {
						FILE *test = gf_fopen(inName, "rb");
						if (!test) {
							open_mode = (do_flat) ? GF_ISOM_OPEN_WRITE : GF_ISOM_WRITE_EDIT;
							if (!outName) outName = inName;
						}
						else gf_fclose(test);
					}

					open_edit = GF_TRUE;
					file = gf_isom_open(inName, open_mode, NULL);
					if (!file) {
						M4_LOG(GF_LOG_ERROR, ("Cannot open destination file %s: %s\n", inName, gf_error_to_string(gf_isom_last_error(NULL)) ));
						return mp4box_cleanup(1);
					}
				}

				e = cat_isomedia_file(file, argv[i+1], import_flags, import_fps, agg_samples, force_cat, align_cat, !strcmp(argv[i], "-catx") ? GF_TRUE : GF_FALSE, !strcmp(argv[i], "-catpl") ? GF_TRUE : GF_FALSE);
				if (e) {
					M4_LOG(GF_LOG_ERROR, ("Error appending %s: %s\n", argv[i+1], gf_error_to_string(e)));
					gf_isom_delete(file);
					return mp4box_cleanup(1);
				}
				i++;
			}
		}
		if ((nb_pass == 2) && !ipass) {
			GF_Filter *mux_filter = gf_fs_load_filter(fs, mux_args, NULL);
			gf_free(mux_args);
			if (!mux_filter) {
				M4_LOG(GF_LOG_ERROR, ("Error loadin isobmff mux filter\n"));
				gf_isom_delete(file);
				gf_fs_del(fs);
				return mp4box_cleanup(1);
			}

			e = gf_fs_run(fs);
			if (e==GF_EOS) e = GF_OK;

			if (!e) e = gf_fs_get_last_connect_error(fs);
			if (!e) e = gf_fs_get_last_process_error(fs);

			if (e) {
				M4_LOG(GF_LOG_ERROR, ("Error importing sources: %s\n", gf_error_to_string(e)));
				gf_isom_delete(file);
				gf_fs_del(fs);
				return mp4box_cleanup(1);
			}
		}
	}
	if (fs) {
		gf_fs_print_non_connected(fs);
		if (fs_dump_flags & 1) gf_fs_print_stats(fs);
		if (fs_dump_flags & 2) gf_fs_print_connections(fs);
		gf_fs_del(fs);
	}

	/*unless explicitly asked, remove all systems tracks*/
#ifndef GPAC_DISABLE_AV_PARSERS
	if (!keep_sys_tracks) remove_systems_tracks(file);
#endif
	do_save = GF_TRUE;

	return 0;
}