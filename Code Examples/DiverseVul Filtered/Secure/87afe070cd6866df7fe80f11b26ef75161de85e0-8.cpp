int mp4boxMain(int argc, char **argv)
{
	u32 i, j;
	const char *gpac_profile = "0";
	GF_Err e = GF_OK;

#ifdef TEST_ARGS
	i=0;
	mp4box_parse_single_arg(argc, argv, "", &i);
#endif

	for (i = 1; i < (u32) argc ; i++) {
		if (!strcmp(argv[i], "-mem-track") || !strcmp(argv[i], "-mem-track-stack")) {
#ifdef GPAC_MEMORY_TRACKING
            mem_track = !strcmp(argv[i], "-mem-track-stack") ? GF_MemTrackerBackTrace : GF_MemTrackerSimple;
#else
			M4_LOG(GF_LOG_WARNING, ("WARNING - GPAC not compiled with Memory Tracker - ignoring \"%s\"\n", argv[i]));
#endif
			break;
		}
		else if (!strcmp(argv[i], "-p")) {
			if (i+1<(u32) argc)
				gpac_profile = argv[i+1];
			else {
				M4_LOG(GF_LOG_ERROR, ("Bad argument for -p, expecting profile name but no more args\n"));
				return 1;
			}
		}
		else if (!strncmp(argv[i], "-p=", 3))
			gpac_profile = argv[i]+3;
	}

#ifdef _TWO_DIGIT_EXPONENT
	_set_output_format(_TWO_DIGIT_EXPONENT);
#endif

	/*init libgpac*/
	gf_sys_init(mem_track, gpac_profile);
	if (argc < 2) {
		M4_LOG(GF_LOG_ERROR, ("Not enough arguments - check usage with -h\n"));
		M4_LOG(GF_LOG_INFO, ("MP4Box - GPAC version %s\n"
	        "%s\n", gf_gpac_version(), gf_gpac_copyright_cite() ));
		gf_sys_close();
		return 0;
	}

	helpout = stdout;

	i = mp4box_parse_args(argc, argv);
	if (i) {
		return mp4box_cleanup(i - 1);
	}
#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
	if (live_scene) {
		int ret = live_session(argc, argv);
		return mp4box_cleanup(ret);
	}
#endif

	if (!dash_duration && interleaving_time && do_frag)
		interleaving_time /= 1000;

	if (do_mpd_conv) inName = do_mpd_conv;

	if (import_flags & GF_IMPORT_FORCE_MPEG4)
		hint_flags |= GP_RTP_PCK_FORCE_MPEG4;

	if (!inName && dump_std)
		inName = "std";

	if (!dash_duration && cprt)
		open_edit = GF_TRUE;

	if (!inName) {
		if (has_next_arg) {
			M4_LOG(GF_LOG_ERROR, ("Broken argument specifier or file name missing - check usage with -h\n"));
		} else {
			PrintUsage();
		}
		return mp4box_cleanup(1);
	}
	if (!strcmp(inName, "std")) dump_std = 2;
	if (!strcmp(inName, "stdb")) {
		inName = "std";
		dump_std = 1;
	}

	if (!interleaving_time) {
		/*by default use single fragment per dash segment*/
		if (dash_duration)
			interleaving_time = dash_duration;
		else if (!do_flat) {
			interleaving_time = DEFAULT_INTERLEAVING_IN_SEC;
		}
	}

	if (dump_std)
		outName = "std";

	if (dump_std==2) {
#ifdef WIN32
		if ( _setmode(_fileno(stdout), _O_BINARY) == -1 )
#else
		if ( freopen(NULL, "wb", stdout) == NULL)
#endif
		{
			M4_LOG(GF_LOG_ERROR, ("Fatal error: cannot reopen stdout in binary mode.\n"));
			return mp4box_cleanup(1);
		}
	}

	GF_LOG_Level level = verbose ? GF_LOG_DEBUG : GF_LOG_INFO;
	gf_log_set_tool_level(GF_LOG_CONTAINER, level);
	gf_log_set_tool_level(GF_LOG_SCENE, level);
	gf_log_set_tool_level(GF_LOG_PARSER, level);
	gf_log_set_tool_level(GF_LOG_AUTHOR, level);
	gf_log_set_tool_level(GF_LOG_CODING, level);
	gf_log_set_tool_level(GF_LOG_DASH, level);
#ifdef GPAC_MEMORY_TRACKING
	if (mem_track)
		gf_log_set_tool_level(GF_LOG_MEMORY, level);
#endif

	e = gf_sys_set_args(argc, (const char **) argv);
	if (e) {
		M4_LOG(GF_LOG_ERROR, ("Error assigning libgpac arguments: %s\n", gf_error_to_string(e) ));
		return mp4box_cleanup(1);
	}

	if (raw_cat)
		return do_raw_cat();

	if (compress_top_boxes) {
		if (size_top_box) {
			u64 top_size = do_size_top_boxes(inName, compress_top_boxes, size_top_box);
			fprintf(stdout, LLU"\n", top_size);
			return mp4box_cleanup(e ? 1 : 0);
		} else {
			e = do_compress_top_boxes(inName, outName);
			return mp4box_cleanup(e ? 1 : 0);
		}
	}

	if (do_mpd_rip) {
		e = rip_mpd(inName, outName);
		return mp4box_cleanup(e ? 1 : 0);
	}

#ifndef GPAC_DISABLE_CORE_TOOLS
	if (do_wget != NULL) {
		e = gf_dm_wget(do_wget, inName, 0, 0, NULL);
		if (e != GF_OK) {
			M4_LOG(GF_LOG_ERROR, ("Cannot retrieve %s: %s\n", do_wget, gf_error_to_string(e) ));
			return mp4box_cleanup(1);
		}
		return mp4box_cleanup(0);
	}
#endif

	if (udp_dest)
		return do_write_udp();

#ifndef GPAC_DISABLE_MPD
	if (do_mpd_conv)
		return convert_mpd();
#endif

	if (dash_duration && !nb_dash_inputs) {
		dash_inputs = set_dash_input(dash_inputs, inName, &nb_dash_inputs);
	}

	if (do_saf && !encode) {
		switch (get_file_type_by_ext(inName)) {
		case GF_FILE_TYPE_BT_WRL_X3DV:
		case GF_FILE_TYPE_XMT_X3D:
		case GF_FILE_TYPE_SVG:
			encode = GF_TRUE;
			break;
		case GF_FILE_TYPE_NOT_SUPPORTED:
		case GF_FILE_TYPE_ISO_MEDIA:
		case GF_FILE_TYPE_SWF:
		case GF_FILE_TYPE_LSR_SAF:
			break;
		}
	}

#ifndef GPAC_DISABLE_SCENE_DUMP
	if (dump_mode == GF_SM_DUMP_SVG) {
		if (strstr(inName, ".srt") || strstr(inName, ".ttxt")) import_subtitle = 2;
	}
#endif

	if (import_subtitle && !trackID)
		return do_import_sub();


#if !defined(GPAC_DISABLE_MEDIA_IMPORT) && !defined(GPAC_DISABLE_ISOM_WRITE)
	if (nb_add || nb_cat) {
		u32 res = do_add_cat(argc, argv);
		if (res) return res;
	}
#endif /*!GPAC_DISABLE_MEDIA_IMPORT && !GPAC_DISABLE_ISOM_WRITE*/

#if !defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_SCENE_ENCODER) && !defined(GPAC_DISABLE_MEDIA_IMPORT)
	else if (chunk_mode) {
		if (!inName) {
			M4_LOG(GF_LOG_ERROR, ("chunk encoding syntax: [-outctx outDump] -inctx inScene auFile\n"));
			return mp4box_cleanup(1);
		}
		e = EncodeFileChunk(inName, outName ? outName : inName, input_ctx, output_ctx);
		if (e) {
			M4_LOG(GF_LOG_ERROR, ("Error encoding chunk file %s\n", gf_error_to_string(e)));
			return mp4box_cleanup(1);
		}
		goto exit;
	}
#endif // !defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_SCENE_ENCODER) && !defined(GPAC_DISABLE_MEDIA_IMPORT)

	else if (encode) {
#if !defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_SCENE_ENCODER) && !defined(GPAC_DISABLE_MEDIA_IMPORT)
		e = do_scene_encode();
		if (e) goto err_exit;
#endif //!defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_SCENE_ENCODER) && !defined(GPAC_DISABLE_MEDIA_IMPORT)
	}

#ifndef GPAC_DISABLE_ISOM_WRITE
	else if (pack_file) {
		char *fileName = gf_url_colon_suffix(pack_file);
		if (fileName && ((fileName - pack_file)==4)) {
			fileName[0] = 0;
			file = package_file(fileName + 1, pack_file, pack_wgt);
			fileName[0] = ':';
		} else {
			file = package_file(pack_file, NULL, pack_wgt);
			if (!file) {
				M4_LOG(GF_LOG_ERROR, ("Failed to package file\n"));
				return mp4box_cleanup(1);
			}
		}
		if (!outName) outName = inName;
		do_save = GF_TRUE;
		open_edit = GF_TRUE;
	}
#endif //GPAC_DISABLE_ISOM_WRITE

	if (dash_duration) {
		e = do_dash();
		if (e) return mp4box_cleanup(1);
		goto exit;
	}

	//need to open input
	if (!file && !do_hash) {
		FILE *st = gf_fopen(inName, "rb");
		Bool file_exists = 0;
		GF_ISOOpenMode omode;
		if (st) {
			file_exists = 1;
			gf_fclose(st);
		}
		switch (get_file_type_by_ext(inName)) {
		case 1:
			omode =  (u8) (force_new ? GF_ISOM_WRITE_EDIT : (open_edit ? GF_ISOM_OPEN_EDIT : ( ((dump_isom>0) || print_info) ? GF_ISOM_OPEN_READ_DUMP : GF_ISOM_OPEN_READ) ) );

			if (crypt) {
				//keep fragment signaling in moov
				omode = GF_ISOM_OPEN_READ;
				if (use_init_seg)
					file = gf_isom_open(use_init_seg, GF_ISOM_OPEN_READ, NULL);
			}
			if (!crypt && use_init_seg) {
				file = gf_isom_open(use_init_seg, GF_ISOM_OPEN_READ_DUMP, NULL);
				if (file) {
					e = gf_isom_open_segment(file, inName, 0, 0, 0);
					if (e==GF_ISOM_INCOMPLETE_FILE) {
						M4_LOG(GF_LOG_WARNING, ("Segment %s: %s\n", inName, gf_error_to_string(e) ));
					} else if (e) {
						M4_LOG(GF_LOG_ERROR, ("Error opening segment %s: %s\n", inName, gf_error_to_string(e) ));
						gf_isom_delete(file);
						file = NULL;
					}
				}
			}
			if (!file)
				file = gf_isom_open(inName, omode, NULL);

			if (!file && (gf_isom_last_error(NULL) == GF_ISOM_INCOMPLETE_FILE) && !open_edit) {
				u64 missing_bytes;
				gf_isom_open_progressive(inName, 0, 0, GF_FALSE, &file, &missing_bytes);
				if (missing_bytes)
					M4_LOG(GF_LOG_ERROR, ("Truncated file - missing "LLD" bytes\n", missing_bytes));
			}

			if (!file) {
				if (open_edit && nb_meta_act) {
					file = gf_isom_open(inName, GF_ISOM_WRITE_EDIT, NULL);
					if (!outName && file) outName = inName;
				}

				if (!file) {
					M4_LOG(GF_LOG_ERROR, ("Error opening file %s: %s\n", inName, gf_error_to_string(gf_isom_last_error(NULL))));
					return mp4box_cleanup(1);
				}
			}
			if (freeze_box_order)
				gf_isom_freeze_order(file);
			break;
		/*allowed for bt<->xmt*/
		case 2:
		case 3:
		/*allowed for svg->lsr**/
		case 4:
		/*allowed for swf->bt, swf->xmt, swf->svg*/
		case 5:
			break;
		/*used for .saf / .lsr dump*/
		case 6:
#ifndef GPAC_DISABLE_SCENE_DUMP
			if ((dump_mode==GF_SM_DUMP_LASER) || (dump_mode==GF_SM_DUMP_SVG)) {
				break;
			}
#endif

		default:
			if (!open_edit && file_exists && !gf_isom_probe_file(inName) && track_dump_type) {
			}
#ifndef GPAC_DISABLE_ISOM_WRITE
			else if (!open_edit && file_exists /* && !gf_isom_probe_file(inName) */
#ifndef GPAC_DISABLE_SCENE_DUMP
			         && dump_mode == GF_SM_DUMP_NONE
#endif //GPAC_DISABLE_SCENE_DUMP
			        ) {
				/*************************************************************************************************/
#ifndef GPAC_DISABLE_MEDIA_IMPORT
				if(dvbhdemux)
				{
					GF_MediaImporter import;
					file = gf_isom_open("ttxt_convert", GF_ISOM_OPEN_WRITE, NULL);
					memset(&import, 0, sizeof(GF_MediaImporter));
					import.dest = file;
					import.in_name = inName;
					import.flags = GF_IMPORT_MPE_DEMUX;
					e = gf_media_import(&import);
					if (e) {
						M4_LOG(GF_LOG_ERROR, ("Error importing %s: %s\n", inName, gf_error_to_string(e)));
						gf_isom_delete(file);
						gf_file_delete("ttxt_convert");
						return mp4box_cleanup(1);
					}
				}
#endif /*GPAC_DISABLE_MEDIA_IMPORT*/

				if (dump_m2ts) {
#ifndef GPAC_DISABLE_MPEG2TS
					dump_mpeg2_ts(inName, outName, program_number);
#endif
				} else if (dump_timestamps) {
#ifndef GPAC_DISABLE_MPEG2TS
					dump_mpeg2_ts(inName, outName, program_number);
#endif
#ifndef GPAC_DISABLE_CORE_TOOLS
				} else if (do_bin_xml) {
					xml_bs_to_bin(inName, outName, dump_std);
#endif
				} else if (do_hash) {
					hash_file(inName, dump_std);
				} else if (print_info) {
#ifndef GPAC_DISABLE_MEDIA_IMPORT
					convert_file_info(inName, info_track_id);
#endif
				} else {
					if (mux_name) {
						e = do_remux_file();
						if (e) goto err_exit;
						if (file) gf_isom_delete(file);
						goto exit;
					} else {
						M4_LOG(GF_LOG_ERROR, ("Input %s is not an MP4 file, operation not allowed\n", inName));
						return mp4box_cleanup(1);
					}
				}
				goto exit;
			}
#endif /*GPAC_DISABLE_ISOM_WRITE*/
			else if (open_edit) {
				file = gf_isom_open(inName, GF_ISOM_WRITE_EDIT, NULL);
				if (!outName && file) outName = inName;
			} else if (!file_exists) {
				M4_LOG(GF_LOG_ERROR, ("Error %s file %s: %s\n", force_new ? "creating" : "opening", inName, gf_error_to_string(GF_URL_ERROR)));
				return mp4box_cleanup(1);
			} else {
				M4_LOG(GF_LOG_ERROR, ("Cannot open %s - extension not supported\n", inName));
				return mp4box_cleanup(1);
			}
		}
	}

	if (high_dynamc_range_filename) {
		e = parse_high_dynamc_range_xml_desc(file, high_dynamc_range_filename);
		if (e) goto err_exit;
	}

	if (file && keep_utc) {
		gf_isom_keep_utc_times(file, 1);
	}

	if ( gf_strlcpy(outfile, outName ? outName : inName, sizeof(outfile)) >= sizeof(outfile) ) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("Filename too long (limit is %d)\n", GF_MAX_PATH));
		return mp4box_cleanup(1);
	}

	char *szExt = gf_file_ext_start(outfile);
	if (szExt) {
		/*turn on 3GP saving*/
		if (!stricmp(szExt, ".3gp") || !stricmp(szExt, ".3gpp") || !stricmp(szExt, ".3g2"))
			conv_type = GF_ISOM_CONV_TYPE_3GPP;
		else if (!stricmp(szExt, ".m4a") || !stricmp(szExt, ".m4v"))
			conv_type = GF_ISOM_CONV_TYPE_IPOD;
		else if (!stricmp(szExt, ".psp"))
			conv_type = GF_ISOM_CONV_TYPE_PSP;
		else if (!stricmp(szExt, ".mov") || !stricmp(szExt, ".qt"))
			conv_type = GF_ISOM_CONV_TYPE_MOV;

		//remove extension from outfile
		*szExt = 0;
	}

#ifndef GPAC_DISABLE_MEDIA_EXPORT
	if (!open_edit && track_dump_type && !gf_isom_probe_file(inName)) {
		e = do_export_tracks_non_isobmf();
		if (e) goto err_exit;
		goto exit;
	}
	if (mux_name) {
		e = do_remux_file();
		if (e) goto err_exit;
		if (file) gf_isom_delete(file);
		goto exit;
	}



#endif /*GPAC_DISABLE_MEDIA_EXPORT*/

#ifndef GPAC_DISABLE_SCENE_DUMP
	if (dump_mode != GF_SM_DUMP_NONE) {
		e = dump_isom_scene(inName, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, dump_mode, do_scene_log, no_odf_conf);
		if (e) goto err_exit;
	}
#endif

#ifndef GPAC_DISABLE_SCENE_STATS
	if (stat_level) dump_isom_scene_stats(inName, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, stat_level);
#endif

#ifndef GPAC_DISABLE_ISOM_HINTING
	if (!do_hint && print_sdp) dump_isom_sdp(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);
#endif
	if (get_nb_tracks) {
		fprintf(stdout, "%d\n", gf_isom_get_track_count(file));
	}
	if (print_info) {
		if (!file) {
			M4_LOG(GF_LOG_ERROR, ("Cannot print info on a non ISOM file (%s)\n", inName));
		} else {
			if (info_track_id) DumpTrackInfo(file, info_track_id, 1, (print_info==2) ? GF_TRUE : GF_FALSE, GF_FALSE);
			else DumpMovieInfo(file);
		}
	}
#ifndef GPAC_DISABLE_ISOM_DUMP
	if (dump_isom) {
		e = dump_isom_xml(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, (dump_isom==2) ? GF_TRUE : GF_FALSE, merge_vtt_cues, use_init_seg ? GF_TRUE : GF_FALSE, (dump_isom==3) ? GF_TRUE : GF_FALSE);
		if (e) goto err_exit;
	}
	if (dump_cr) dump_isom_ismacryp(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);
	if ((dump_ttxt || dump_srt) && trackID) {

		if (trackID == (u32)-1) {
			for (j=0; j<gf_isom_get_track_count(file); j++) {
				trackID = gf_isom_get_track_id(file, j+1);
				dump_isom_timed_text(file, trackID, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE,
									GF_FALSE, dump_srt ? GF_TEXTDUMPTYPE_SRT : GF_TEXTDUMPTYPE_TTXT);
			}

		}
		else {
			dump_isom_timed_text(file, trackID, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE,
								GF_FALSE, dump_srt ? GF_TEXTDUMPTYPE_SRT : GF_TEXTDUMPTYPE_TTXT);
		}
	}

#ifndef GPAC_DISABLE_ISOM_HINTING
	if (dump_rtp) dump_isom_rtp(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);
#endif

#endif

	if (dump_timestamps) dump_isom_timestamps(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, dump_timestamps);
	if (dump_nal) dump_isom_nal(file, dump_nal, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, dump_nal_type);
	if (dump_saps) dump_isom_saps(file, dump_saps, dump_saps_mode, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);

	if (do_hash) {
		e = hash_file(inName, dump_std);
		if (e) goto err_exit;
	}
#ifndef GPAC_DISABLE_CORE_TOOLS
	if (do_bin_xml) {
		e = xml_bs_to_bin(inName, outName, dump_std);
		if (e) goto err_exit;
	}
#endif

	if (dump_cart) dump_isom_cover_art(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);
	if (dump_chap) dump_isom_chapters(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, dump_chap);
	if (dump_udta_type) dump_isom_udta(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE, dump_udta_type, dump_udta_track);

	if (dump_iod) {
		e = do_dump_iod();
		if (e) goto err_exit;
	}

#if !defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_MEDIA_IMPORT)
	if (split_duration || split_size || split_range_str) {
		split_isomedia_file(file, split_duration, split_size, inName, interleaving_time, split_start, adjust_split_end, outName, seg_at_rap, split_range_str, fs_dump_flags);

		/*never save file when splitting is desired*/
		open_edit = GF_FALSE;
		do_save = GF_FALSE;
	}
#endif // !defined(GPAC_DISABLE_ISOM_WRITE) && !defined(GPAC_DISABLE_MEDIA_IMPORT)

#ifndef GPAC_DISABLE_MEDIA_EXPORT
	if (track_dump_type) {
		e = do_export_tracks();
		if (e) goto err_exit;
	} else if (do_saf) {
		GF_MediaExporter mdump;
		memset(&mdump, 0, sizeof(mdump));
		mdump.file = file;
		mdump.flags = GF_EXPORT_SAF;
		mdump.out_name = outfile;
		mdump.print_stats_graph = fs_dump_flags;
		e = gf_media_export(&mdump);
		if (e) goto err_exit;
	}
#endif

	e = do_meta_act();
	if (e) goto err_exit;

	if (!open_edit && !do_save) {
		if (file) gf_isom_delete(file);
		goto exit;
	}


#ifndef GPAC_DISABLE_ISOM_WRITE
	if (clean_groups) {
		e = gf_isom_reset_switch_parameters(file);
		if (e) goto err_exit;
		do_save = GF_TRUE;
	}


	e = do_tsel_act();
	if (e) goto err_exit;

	if (remove_sys_tracks) {
#ifndef GPAC_DISABLE_AV_PARSERS
		remove_systems_tracks(file);
#endif
		do_save = GF_TRUE;
		if (conv_type < GF_ISOM_CONV_TYPE_ISMA_EX) conv_type = 0;
	}
	if (remove_root_od) {
		gf_isom_remove_root_od(file);
		do_save = GF_TRUE;
	}
#ifndef GPAC_DISABLE_ISOM_HINTING
	if (remove_hint) {
		for (i=0; i<gf_isom_get_track_count(file); i++) {
			if (gf_isom_get_media_type(file, i+1) == GF_ISOM_MEDIA_HINT) {
				M4_LOG(GF_LOG_INFO, ("Removing hint track ID %d\n", gf_isom_get_track_id(file, i+1)));
				gf_isom_remove_track(file, i+1);
				i--;
			}
		}
		gf_isom_sdp_clean(file);
		do_save = GF_TRUE;
	}
#endif // GPAC_DISABLE_ISOM_HINTING

	if (timescale && (timescale != gf_isom_get_timescale(file))) {
		gf_isom_set_timescale(file, timescale);
		do_save = GF_TRUE;
	}

	if (!encode) {
		if (!file) {
			M4_LOG(GF_LOG_INFO, ("Nothing to do - exiting\n"));
			goto exit;
		}
		if (outName) {
			strcpy(outfile, outName);
		} else {
			const char *tmp_dir = gf_opts_get_key("core", "tmp");
			char *rel_name = strrchr(inName, GF_PATH_SEPARATOR);
			if (!rel_name) rel_name = strrchr(inName, '/');

			strcpy(outfile, "");
			if (tmp_dir) {
				strcpy(outfile, tmp_dir);
				if (!strchr("\\/", tmp_dir[strlen(tmp_dir)-1])) strcat(outfile, "/");
			}
			if (!pack_file) strcat(outfile, "out_");
			strcat(outfile, rel_name ? rel_name + 1 : inName);

			if (pack_file) {
				strcpy(outfile, rel_name ? rel_name + 1 : inName);
				rel_name = strrchr(outfile, '.');
				if (rel_name) rel_name[0] = 0;
				strcat(outfile, ".m21");
			}
		}
#ifndef GPAC_DISABLE_MEDIA_IMPORT
		if ((conv_type == GF_ISOM_CONV_TYPE_ISMA) || (conv_type == GF_ISOM_CONV_TYPE_ISMA_EX)) {
			M4_LOG(GF_LOG_INFO, ("Converting to ISMA Audio-Video MP4 file\n"));
			/*keep ESIDs when doing ISMACryp*/
			e = gf_media_make_isma(file, crypt ? 1 : 0, GF_FALSE, (conv_type==GF_ISOM_CONV_TYPE_ISMA_EX) ? 1 : 0);
			if (e) goto err_exit;
			do_save = GF_TRUE;
		}
		if (conv_type == GF_ISOM_CONV_TYPE_3GPP) {
			M4_LOG(GF_LOG_INFO, ("Converting to 3GP file\n"));
			e = gf_media_make_3gpp(file);
			if (e) goto err_exit;
			do_save = GF_TRUE;
		}
		if (conv_type == GF_ISOM_CONV_TYPE_PSP) {
			M4_LOG(GF_LOG_INFO, ("Converting to PSP file\n"));
			e = gf_media_make_psp(file);
			if (e) goto err_exit;
			do_save = GF_TRUE;
		}
		if (conv_type == GF_ISOM_CONV_TYPE_MOV) {
			e = gf_media_check_qt_prores(file);
			if (e) goto err_exit;
			do_save = GF_TRUE;
			if (interleaving_time) interleaving_time = 0.5;
		}
#endif /*GPAC_DISABLE_MEDIA_IMPORT*/
		if (conv_type == GF_ISOM_CONV_TYPE_IPOD) {
			do_ipod_conv();
		}

	} else if (outName) {
		strcpy(outfile, outName);
	}

	e = do_track_act();
	if (e) goto err_exit;

	if (itunes_tags) {
		e = do_itunes_tag();
		if (e) goto err_exit;
	}

	if (cprt) {
		e = gf_isom_set_copyright(file, "und", cprt);
		do_save = GF_TRUE;
		if (e) goto err_exit;
	}
	if (chap_file || chap_file_qt) {
#ifndef GPAC_DISABLE_MEDIA_IMPORT
		Bool chap_qt = GF_FALSE;
		if (chap_file_qt) {
			chap_file = chap_file_qt;
			chap_qt = GF_TRUE;
		}
		e = gf_media_import_chapters(file, chap_file, import_fps, chap_qt);
		do_save = GF_TRUE;
#else
		M4_LOG(GF_LOG_WARNING, ("Warning: GPAC compiled without Media Import, chapters can't be imported\n"));
		e = GF_NOT_SUPPORTED;
#endif
		if (e) goto err_exit;
	}

	if (major_brand) {
		gf_isom_set_brand_info(file, major_brand, minor_version);
		do_save = GF_TRUE;
	}
	for (i=0; i<nb_alt_brand_add; i++) {
		gf_isom_modify_alternate_brand(file, brand_add[i], GF_TRUE);
		do_save = GF_TRUE;
	}
	for (i=0; i<nb_alt_brand_rem; i++) {
		gf_isom_modify_alternate_brand(file, brand_rem[i], GF_FALSE);
		do_save = GF_TRUE;
	}
	if (box_patch_filename) {
		e = gf_isom_apply_box_patch(file, box_patch_trackID, box_patch_filename, GF_FALSE);
		if (e) {
			M4_LOG(GF_LOG_ERROR, ("Failed to apply box patch %s: %s\n", box_patch_filename, gf_error_to_string(e) ));
			goto err_exit;
		}
		do_save = GF_TRUE;
	}

#ifndef GPAC_DISABLE_CRYPTO
	if (crypt) {
		if (!drm_file && (crypt==1) ) {
			M4_LOG(GF_LOG_ERROR, ("Missing DRM file location - usage '-%s drm_file input_file\n", (crypt==1) ? "crypt" : "decrypt"));
			e = GF_BAD_PARAM;
			goto err_exit;
		}
		if (crypt == 1) {
			if (use_init_seg) {
				e = gf_crypt_fragment(file, drm_file, outfile, inName, fs_dump_flags);
			} else {
				e = gf_crypt_file(file, drm_file, outfile, interleaving_time, fs_dump_flags);
			}
		} else if (crypt ==2) {
			if (use_init_seg) {
				e = gf_decrypt_fragment(file, drm_file, outfile, inName, fs_dump_flags);
			} else {
				e = gf_decrypt_file(file, drm_file, outfile, interleaving_time, fs_dump_flags);
			}
		}
		if (e) goto err_exit;
		do_save = outName ? GF_FALSE : GF_TRUE;

		if (!do_frag && !do_hint && !full_interleave && !force_co64) {
			char szName[GF_MAX_PATH];
			strcpy(szName, gf_isom_get_filename(file) );
			gf_isom_delete(file);
			file = NULL;
			if (!outName) {
				e = gf_file_move(outfile, szName);
				if (e) goto err_exit;
			}
			goto exit;
		}
	}
#endif /*GPAC_DISABLE_CRYPTO*/

#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (do_frag) {
		if (!interleaving_time) interleaving_time = DEFAULT_INTERLEAVING_IN_SEC;
		if (do_hint) M4_LOG(GF_LOG_WARNING, ("Warning: cannot hint and fragment - ignoring hint\n"));
		M4_LOG(GF_LOG_INFO, ("Fragmenting file (%.3f seconds fragments)\n", interleaving_time));
		e = gf_media_fragment_file(file, outfile, interleaving_time, use_mfra);
		if (e) M4_LOG(GF_LOG_ERROR, ("Error while fragmenting file: %s\n", gf_error_to_string(e)));
		if (!e && !outName) {
			if (gf_file_exists(inName) && gf_file_delete(inName)) {
				M4_LOG(GF_LOG_INFO, ("Error removing file %s\n", inName));
			}
			else if (gf_file_move(outfile, inName)) {
				M4_LOG(GF_LOG_INFO, ("Error renaming file %s to %s\n", outfile, inName));
			}
		}
		if (e) goto err_exit;
		gf_isom_delete(file);
		goto exit;
	}
#endif

#ifndef GPAC_DISABLE_ISOM_HINTING
	if (do_hint) {
		if (force_ocr) SetupClockReferences(file);
		MTUSize -= 12;
		e = HintFile(file, MTUSize, max_ptime, rtp_rate, hint_flags, HintCopy, hint_interleave, regular_iod, single_group, hint_no_offset);
		if (e) goto err_exit;
		do_save = GF_TRUE;
		if (print_sdp) dump_isom_sdp(file, dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE);
	}
#endif

#if !defined(GPAC_DISABLE_ISOM_HINTING) && !defined(GPAC_DISABLE_SENG)
	set_sdp_ext();
#endif /*!defined(GPAC_DISABLE_ISOM_HINTING) && !defined(GPAC_DISABLE_SENG)*/

	if (force_co64)
		gf_isom_force_64bit_chunk_offset(file, GF_TRUE);

	if (compress_moov)
		gf_isom_enable_compression(file, GF_ISO_COMP_MOOV, GF_FALSE);

	if (no_inplace)
		gf_isom_disable_inplace_rewrite(file);

	if (moov_pading)
		gf_isom_set_inplace_padding(file, moov_pading);

	if (outName) {
		gf_isom_set_final_name(file, outfile);
	} else if (!encode && !force_new && !gf_isom_is_inplace_rewrite(file)) {
		gf_isom_set_final_name(file, outfile);
	}

	Bool is_inplace = gf_isom_is_inplace_rewrite(file);


	/*full interleave (sample-based) if just hinted*/
	if (full_interleave) {
		e = gf_isom_set_storage_mode(file, GF_ISOM_STORE_TIGHT);
	} else if (do_flat) {
		e = gf_isom_set_storage_mode(file, (do_flat==1) ? GF_ISOM_STORE_FLAT : GF_ISOM_STORE_STREAMABLE);
		do_save = GF_TRUE;
	}
	//do not set storage mode unless inplace rewrite is disabled , either by user or due to operations on file
	else if (!is_inplace) {
		e = gf_isom_make_interleave(file, interleaving_time);
		if (!e && old_interleave) e = gf_isom_set_storage_mode(file, GF_ISOM_STORE_INTERLEAVED);
	}

	if (e) goto err_exit;


	if (do_save) {

		if (!gf_sys_is_quiet()) {
			if (outName) {
			} else if (encode || pack_file) {
				M4_LOG(GF_LOG_INFO, ("Saving to %s: ", gf_isom_get_filename(file) ));
			} else {
				M4_LOG(GF_LOG_INFO, ("Saving %s: ", inName));
			}
			if (is_inplace) {
				M4_LOG(GF_LOG_INFO, ("In-place rewrite\n"));
			} else if (do_hint && full_interleave) {
				M4_LOG(GF_LOG_INFO, ("Hinted file - Full Interleaving\n"));
			} else if (full_interleave) {
				M4_LOG(GF_LOG_INFO, ("Full Interleaving\n"));
			} else if ((force_new==2) && interleaving_time) {
				M4_LOG(GF_LOG_INFO, ("Fast-start interleaved storage\n"));
			} else if (do_flat || !interleaving_time) {
				M4_LOG(GF_LOG_INFO, ("Flat storage\n"));
			} else {
				M4_LOG(GF_LOG_INFO, ("%.3f secs Interleaving%s\n", interleaving_time, old_interleave ? " - no drift control" : ""));
			}
		}

		e = gf_isom_close(file);
		file = NULL;

		if (!e && !outName && !encode && !force_new && !pack_file && !is_inplace) {
			if (gf_file_exists(inName)) {
				e = gf_file_delete(inName);
				if (e) {
					M4_LOG(GF_LOG_ERROR, ("Error removing file %s\n", inName));
				}
			}

			e = gf_file_move(outfile, inName);
			if (e) {
				M4_LOG(GF_LOG_ERROR, ("Error renaming file %s to %s\n", outfile, inName));
			}
		}
	} else {
		gf_isom_delete(file);
	}

	if (e) {
		M4_LOG(GF_LOG_ERROR, ("Error: %s\n", gf_error_to_string(e)));
		goto err_exit;
	}
	goto exit;

#else
	/*close libgpac*/
	gf_isom_delete(file);
	M4_LOG(GF_LOG_ERROR, ("Error: Read-only version of MP4Box.\n"));
	return mp4box_cleanup(1);
#endif //GPAC_DISABLE_ISOM_WRITE


err_exit:
	/*close libgpac*/
	if (file) gf_isom_delete(file);
	M4_LOG(GF_LOG_ERROR, ("\n\tError: %s\n", gf_error_to_string(e)));
	return mp4box_cleanup(1);

exit:
	mp4box_cleanup(0);

#ifdef GPAC_MEMORY_TRACKING
	if (mem_track && (gf_memory_size() || gf_file_handles_count() )) {
		gf_log_set_tool_level(GF_LOG_MEMORY, GF_LOG_INFO);
		gf_memory_print();
		return 2;
	}
#endif
	return 0;
}