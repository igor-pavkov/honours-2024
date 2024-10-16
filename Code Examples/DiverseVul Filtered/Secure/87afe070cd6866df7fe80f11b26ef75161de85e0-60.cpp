static GF_Err do_dash()
{
	GF_Err e;
	u32 i;
	Bool del_file = GF_FALSE;
	char szMPD[GF_MAX_PATH], *sep;
	char szStateFile[GF_MAX_PATH];
	Bool dyn_state_file = GF_FALSE;
	u32 do_abort = 0;
	GF_DASHSegmenter *dasher=NULL;

	if (crypt) {
		M4_LOG(GF_LOG_ERROR, ("MP4Box cannot use -crypt and -dash in the same pass. Please encrypt your content first, or specify encryption filters on dash sources.\n"));
		return GF_BAD_PARAM;
	}

	strcpy(outfile, outName ? outName : gf_url_get_resource_name(inName) );
	sep = strrchr(outfile, '.');
	if (sep) sep[0] = 0;
	if (!outName) strcat(outfile, "_dash");
	strcpy(szMPD, outfile);
	if (outName && sep) {
		sep[0] = '.';
		strcat(szMPD, sep);
	} else {
		strcat(szMPD, ".mpd");
	}

	if ((dash_subduration>0) && (dash_duration > dash_subduration)) {
		M4_LOG(GF_LOG_WARNING, ("Warning: -subdur parameter (%g s) should be greater than segment duration (%g s), using segment duration instead\n", dash_subduration, dash_duration));
		dash_subduration = dash_duration;
	}

	if (dash_mode && dash_live)
		M4_LOG(GF_LOG_INFO, ("Live DASH-ing - press 'q' to quit, 's' to save context and quit\n"));

	if (!dash_ctx_file && dash_live) {
		u32 r1;
		u64 add = (u64) (intptr_t) &dasher;
		add ^= gf_net_get_utc();
		r1 = (u32) add ^ (u32) (add/0xFFFFFFFF);
		r1 ^= gf_rand();
		sprintf(szStateFile, "%s/dasher_%X.xml", gf_get_default_cache_directory(), r1 );
		dash_ctx_file = szStateFile;
		dyn_state_file = GF_TRUE;
	} else if (dash_ctx_file) {
		if (force_new)
			gf_file_delete(dash_ctx_file);
	}

	if (dash_profile==GF_DASH_PROFILE_AUTO)
		dash_profile = dash_mode ? GF_DASH_PROFILE_LIVE : GF_DASH_PROFILE_FULL;

	if (!dash_mode) {
		time_shift_depth = 0;
		mpd_update_time = 0;
	} else if ((dash_profile>=GF_DASH_PROFILE_MAIN) && !use_url_template && !mpd_update_time) {
		/*use a default MPD update of dash_duration sec*/
		mpd_update_time = (Double) (dash_subduration ? dash_subduration : dash_duration);
		M4_LOG(GF_LOG_INFO, ("Using default MPD refresh of %g seconds\n", mpd_update_time));
	}

	if (file && do_save) {
		gf_isom_close(file);
		file = NULL;
		del_file = GF_TRUE;
	}

	/*setup dash*/
	dasher = gf_dasher_new(szMPD, dash_profile, NULL, dash_scale, dash_ctx_file);
	if (!dasher) {
		return mp4box_cleanup(1);
	}
	e = gf_dasher_set_info(dasher, dash_title, cprt, dash_more_info, dash_source, NULL);
	if (e) {
		M4_LOG(GF_LOG_ERROR, ("DASH Error: %s\n", gf_error_to_string(e)));
		gf_dasher_del(dasher);
		return e;
	}

	gf_dasher_set_start_date(dasher, dash_start_date);
	gf_dasher_set_location(dasher, dash_source);
	for (i=0; i < nb_mpd_base_urls; i++) {
		e = gf_dasher_add_base_url(dasher, mpd_base_urls[i]);
		if (e) {
			M4_LOG(GF_LOG_ERROR, ("DASH Error: %s\n", gf_error_to_string(e)));
			gf_dasher_del(dasher);
			return e;
		}
	}

	if (segment_timeline && !use_url_template) {
		M4_LOG(GF_LOG_WARNING, ("DASH Warning: using -segment-timeline with no -url-template. Forcing URL template.\n"));
		use_url_template = GF_TRUE;
	}

	e = gf_dasher_enable_url_template(dasher, (Bool) use_url_template, seg_name, seg_ext, init_seg_ext);
	if (!e) e = gf_dasher_enable_segment_timeline(dasher, segment_timeline);
	if (!e) e = gf_dasher_enable_single_segment(dasher, single_segment);
	if (!e) e = gf_dasher_enable_single_file(dasher, single_file);
	if (!e) e = gf_dasher_set_switch_mode(dasher, bitstream_switching_mode);
	if (!e) e = gf_dasher_set_durations(dasher, dash_duration, interleaving_time, dash_subduration);
	if (!e) e = gf_dasher_enable_rap_splitting(dasher, seg_at_rap, frag_at_rap);
	if (!e) e = gf_dasher_set_segment_marker(dasher, segment_marker);
	if (!e) e = gf_dasher_enable_sidx(dasher, (subsegs_per_sidx>=0) ? 1 : 0, (u32) subsegs_per_sidx, daisy_chain_sidx, use_ssix);
	if (!e) e = gf_dasher_set_dynamic_mode(dasher, dash_mode, mpd_update_time, time_shift_depth, mpd_live_duration);
	if (!e) e = gf_dasher_set_min_buffer(dasher, min_buffer);
	if (!e) e = gf_dasher_set_ast_offset(dasher, ast_offset_ms);
	if (!e) e = gf_dasher_enable_memory_fragmenting(dasher, memory_frags);
	if (!e) e = gf_dasher_set_initial_isobmf(dasher, initial_moof_sn, initial_tfdt);
	if (!e) e = gf_dasher_configure_isobmf_default(dasher, no_fragments_defaults, pssh_mode, samplegroups_in_traf, single_traf_per_moof, tfdt_per_traf, mvex_after_traks, sdtp_in_traf);
	if (!e) e = gf_dasher_enable_utc_ref(dasher, insert_utc);
	if (!e) e = gf_dasher_enable_real_time(dasher, frag_real_time);
	if (!e) e = gf_dasher_set_content_protection_location_mode(dasher, cp_location_mode);
	if (!e) e = gf_dasher_set_profile_extension(dasher, dash_profile_extension);
	if (!e) e = gf_dasher_enable_cached_inputs(dasher, no_cache);
	if (!e) e = gf_dasher_enable_loop_inputs(dasher, ! no_loop);
	if (!e) e = gf_dasher_set_split_mode(dasher, dash_split_mode);
	if (!e) e = gf_dasher_set_last_segment_merge(dasher, merge_last_seg);
	if (!e) e = gf_dasher_set_hls_clock(dasher, hls_clock);
	if (!e && dash_cues) e = gf_dasher_set_cues(dasher, dash_cues, strict_cues);
	if (!e) e = gf_dasher_print_session_info(dasher, fs_dump_flags);
	if (!e)  e = gf_dasher_keep_source_utc(dasher, keep_utc);

	for (i=0; i < nb_dash_inputs; i++) {
		if (!e) e = gf_dasher_add_input(dasher, &dash_inputs[i]);
	}
	if (e) {
		M4_LOG(GF_LOG_ERROR, ("DASH Setup Error: %s\n", gf_error_to_string(e)));
		gf_dasher_del(dasher);
		return e;
	}

	dash_cumulated_time=0;

	while (1) {
		if (run_for && (dash_cumulated_time >= run_for)) {
			M4_LOG(GF_LOG_INFO, ("Done running, computing static MPD\n"));
			do_abort = 3;
		}

		dash_prev_time=gf_sys_clock();
		if (do_abort>=2) {
			e = gf_dasher_set_dynamic_mode(dasher, GF_DASH_DYNAMIC_LAST, 0, time_shift_depth, mpd_live_duration);
		}

		if (!e) e = gf_dasher_process(dasher);
		if (!dash_live && (e==GF_EOS) ) {
			M4_LOG(GF_LOG_INFO, ("Nothing to dash, too early ...\n"));
			e = GF_OK;
		}

		if (do_abort)
			break;

		//this happens when reading file while writing them (local playback of the live session ...)
		if (dash_live && (e==GF_IO_ERR) ) {
			M4_LOG(GF_LOG_WARNING, ("Error dashing file (%s) but continuing ...\n", gf_error_to_string(e) ));
			e = GF_OK;
		}

		if (e) break;

		if (dash_live) {
			u64 ms_in_session=0;
			u32 slept = gf_sys_clock();
			u32 sleep_for = gf_dasher_next_update_time(dasher, &ms_in_session);
			M4_LOG(GF_LOG_INFO, ("Next generation scheduled in %u ms (DASH time "LLU" ms)\r", sleep_for, ms_in_session));
			if (run_for && (ms_in_session>=run_for)) {
				dash_cumulated_time = 1+run_for;
				continue;
			}

			while (1) {
				if (gf_prompt_has_input()) {
					char c = (char) gf_prompt_get_char();
					if (c=='X') {
						do_abort = 1;
						break;
					}
					if (c=='q') {
						do_abort = 2;
						break;
					}
					if (c=='s') {
						do_abort = 3;
						break;
					}
				}

				if (dash_mode == GF_DASH_DYNAMIC_DEBUG) {
					break;
				}
				if (!sleep_for) break;

				gf_sleep(sleep_for/10);
				sleep_for = gf_dasher_next_update_time(dasher, NULL);
				if (sleep_for<=1) {
					dash_now_time=gf_sys_clock();
					dash_cumulated_time+=(dash_now_time-dash_prev_time);
					M4_LOG(GF_LOG_INFO, ("Slept for %d ms before generation, dash cumulated time %d\n", dash_now_time - slept, dash_cumulated_time));
					break;
				}
			}
		} else {
			break;
		}
	}

	gf_dasher_del(dasher);

	if (!run_for && dash_ctx_file && (do_abort==3) && (dyn_state_file) && !gf_sys_is_test_mode() ) {
		char szName[1024];
		M4_LOG(GF_LOG_INFO, ("Enter file name to save dash context:\n"));
		if (scanf("%1023s", szName) == 1) {
			gf_file_move(dash_ctx_file, szName);
		}
	}
	if (e) M4_LOG(GF_LOG_ERROR, ("Error DASHing file: %s\n", gf_error_to_string(e)));
	if (file) gf_isom_delete(file);
	if (del_file)
		gf_file_delete(inName);

	return e;
}