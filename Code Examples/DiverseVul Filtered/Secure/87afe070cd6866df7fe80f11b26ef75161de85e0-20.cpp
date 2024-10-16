static Bool create_new_track_action(char *arg_val, u32 act_type, u32 dump_type)
{
	TrackAction *tka;
	char *param = arg_val;
	tracks = (TrackAction *)gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act+1));
	if (!tracks) return GF_FALSE;

	tka = & tracks[nb_track_act];
	nb_track_act++;

	memset(tka, 0, sizeof(TrackAction) );
	tka->act_type = act_type;
	tka->dump_type = dump_type;
	if (act_type != TRAC_ACTION_RAW_EXTRACT) {
		open_edit = GF_TRUE;
		do_save = GF_TRUE;
	}

	if ((act_type==TRAC_ACTION_SET_ID) || (act_type==TRAC_ACTION_SWAP_ID)) {
		if (sscanf(param, "%d:%u", &tka->trackID, &tka->newTrackID) != 2) {
			M4_LOG(GF_LOG_ERROR, ("Bad format for -set-track-id - expecting \"id1:id2\" got \"%s\"\n", param));
			return GF_FALSE;
		}
		return GF_TRUE;
	}
	if (act_type==TRAC_ACTION_SET_PAR) {
		char *ext;
		ext = strchr(param, '=');
		if (!ext) {
			M4_LOG(GF_LOG_ERROR, ("Bad format for track par - expecting tkID=none or tkID=PAR_NUM:PAR_DEN got %s\n", param));
			return GF_FALSE;
		}
		ext[0] = 0;
		tka->trackID = atoi(param);
		ext[0] = '=';

		if (!stricmp(ext+1, "none"))
			tka->par_num = tka->par_den = 0;
		else if (!stricmp(ext+1, "auto")) {
			tka->par_num = tka->par_den = -1;
			tka->force_par = 1;
		}
		else if (!stricmp(ext+1, "force")) {
			tka->par_num = tka->par_den = 1;
			tka->force_par = 1;
		}
		else {
			if (ext[1]=='w') {
				tka->rewrite_bs = 1;
				ext++;
			}
			if (sscanf(ext+1, "%d:%d", &tka->par_num, &tka->par_den) != 2) {
				M4_LOG(GF_LOG_ERROR, ("Bad format for track par - expecting tkID=PAR_NUM:PAR_DEN got %s\n", param));
				return GF_FALSE;
			}
		}
		return GF_TRUE;
	}
	if (act_type==TRAC_ACTION_SET_CLAP) {
		char *ext = strchr(param, '=');
		if (!ext) {
			M4_LOG(GF_LOG_ERROR, ("Bad format for track clap - expecting tkID=none or tkID=Wn,Wd,Hn,Hd,HOn,HOd,VOn,VOd got %s\n", param));
			return GF_FALSE;
		}
		ext[0] = 0;
		tka->trackID = atoi(param);
		ext[0] = '=';
		if (stricmp(ext + 1, "none")) {
			if (sscanf(ext + 1, "%d,%d,%d,%d,%d,%d,%d,%d", &tka->clap_wnum, &tka->clap_wden, &tka->clap_hnum, &tka->clap_hden, &tka->clap_honum, &tka->clap_hoden, &tka->clap_vonum, &tka->clap_voden) != 8) {

				M4_LOG(GF_LOG_ERROR, ("Bad format for track clap - expecting tkID=none or tkID=Wn,Wd,Hn,Hd,HOn,HOd,VOn,VOd got %s\n", param));
				return GF_FALSE;
			}
		}
		return GF_TRUE;
	}

	if (act_type==TRAC_ACTION_SET_MX) {
		char *ext = strchr(param, '=');
		if (!ext) {
			M4_LOG(GF_LOG_ERROR, ("Bad format for track matrix - expecting ID=none or ID=M1:M2:M3:M4:M5:M6:M7:M8:M9 got %s\n", param));
			return GF_FALSE;
		}
		ext[0] = 0;
		tka->trackID = atoi(param);
		ext[0] = '=';
		if (!stricmp(ext + 1, "none")) {
			memset(tka->mx, 0, sizeof(s32)*9);
			tka->mx[0] = tka->mx[4] = tka->mx[8] = 1;
		} else {
			s32 res;
			if (strstr(ext+1, "0x")) {
				res = sscanf(ext + 1, "0x%d:0x%d:0x%d:0x%d:0x%d:0x%d:0x%d:0x%d:0x%d", &tka->mx[0], &tka->mx[1], &tka->mx[2], &tka->mx[3], &tka->mx[4], &tka->mx[5], &tka->mx[6], &tka->mx[7], &tka->mx[8]);
			} else {
				res = sscanf(ext + 1, "%d:%d:%d:%d:%d:%d:%d:%d:%d", &tka->mx[0], &tka->mx[1], &tka->mx[2], &tka->mx[3], &tka->mx[4], &tka->mx[5], &tka->mx[6], &tka->mx[7], &tka->mx[8]);
			}
			if (res != 9) {
				M4_LOG(GF_LOG_ERROR, ("Bad format for track matrix - expecting ID=none or ID=M1:M2:M3:M4:M5:M6:M7:M8:M9 got %s\n", param));
				return GF_FALSE;
			}
		}
		return GF_TRUE;
	}
	if (act_type==TRAC_ACTION_SET_EDITS) {
		char *ext = strchr(param, '=');
		if (!ext) {
			M4_LOG(GF_LOG_ERROR, ("Bad format for track edits - expecting ID=EDITS got %s\n", param));
			return GF_FALSE;
		}
		ext[0] = 0;
		tka->trackID = atoi(param);
		ext[0] = '=';
		tka->string = gf_strdup(ext+1);
		return GF_TRUE;
	}
	if (act_type==TRAC_ACTION_SET_LANGUAGE) {
		char *ext = strchr(param, '=');
		if (!strnicmp(param, "all=", 4)) {
			strncpy(tka->lang, param + 4, 10-1);
		}
		else if (!ext) {
			strncpy(tka->lang, param, 10-1);
		} else {
			strncpy(tka->lang, ext + 1, 10-1);
			ext[0] = 0;
			tka->trackID = atoi(param);
			ext[0] = '=';
		}
		return GF_TRUE;
	}
	if ((act_type==TRAC_ACTION_SET_KIND) || (act_type==TRAC_ACTION_REM_KIND)) {
		char *ext;
		char *scheme_start = NULL;

		//extract trackID
		if (!strnicmp(param, "all=", 4)) {
			scheme_start = param + 4;
		} else {
			ext = strchr(param, '=');
			if (ext) {
				ext[0] = 0;
				if (sscanf(param, "%d", &tka->trackID) == 1) {
					scheme_start = ext + 1;
				} else {
					scheme_start = param;
				}
				ext[0] = '=';
			} else {
				scheme_start = param;
			}
		}

		//extract scheme and value - if not, remove kind
		if (!scheme_start || !scheme_start[0]) {
			M4_LOG(GF_LOG_ERROR, ("Missing kind scheme - expecting ID=schemeURI=value got %s\n", param));
			return GF_FALSE;
		} else {
			ext = strchr(scheme_start, '=');
			if (!ext) {
				tka->kind_scheme = gf_strdup(scheme_start);
			} else {
				ext[0] = 0;
				tka->kind_scheme = gf_strdup(scheme_start);
				ext[0] = '=';
				tka->kind_value = gf_strdup(ext + 1);
			}
		}
		return GF_TRUE;
	}
	if (act_type==TRAC_ACTION_SET_DELAY) {
		char *ext = strchr(param, '=');
		if (!ext) {
			M4_LOG(GF_LOG_ERROR, ("Bad format for track delay - expecting tkID=DLAY got %s\n", param));
			return GF_FALSE;
		}
		ext[0] = 0;
		tka->trackID = atoi(param);
		ext[0] = '=';
		if (sscanf(ext+1, "%d/%u", &tka->delay.num, &tka->delay.den) != 2) {
			tka->delay.num = atoi(ext + 1);
			tka->delay.den = 1000;
		}
		return GF_TRUE;
	}
	if (act_type==TRAC_ACTION_REFERENCE) {
		char *ext = strchr(param, '=');
		if (!ext) ext = strchr(param, ':');
		if (!ext) {
			M4_LOG(GF_LOG_ERROR, ("Bad format for track reference - expecting tkID:XXXX:refID got %s\n", param));
			return GF_FALSE;
		}
		ext[0] = 0;
		tka->trackID = atoi(param);
		ext[0] = '=';

		char *ext2 = strchr(ext, ':');
		if (!ext2) {
			M4_LOG(GF_LOG_ERROR, ("Bad format for track reference - expecting tkID:XXXX:refID got %s\n", param));
			return GF_FALSE;
		}
		ext2[0] = 0;
		strncpy(tka->lang, ext+1, 9);
		ext2[0] = ':';
		tka->newTrackID = (s32) atoi(ext2 + 1);
		return GF_TRUE;
	}
	if (act_type==TRAC_ACTION_SET_HANDLER_NAME) {
		char *ext = strchr(param, '=');
		if (!ext) {
			M4_LOG(GF_LOG_ERROR, ("Bad format for track name - expecting tkID=name got %s\n", param));
			return GF_FALSE;
		}
		ext[0] = 0;
		tka->trackID = atoi(param);
		ext[0] = '=';
		tka->hdl_name = ext + 1;
		return GF_TRUE;
	}
	if (act_type==TRAC_ACTION_SET_KMS_URI) {
		char *ext = strchr(param, '=');

		if (!strnicmp(param, "all=", 4)) {
			tka->kms = param + 4;
		} else if (!ext) {
			tka->kms = param;
		} else {
			tka->kms = ext + 1;
			ext[0] = 0;
			tka->trackID = atoi(param);
			ext[0] = '=';
		}
		return GF_TRUE;
	}
	if ((act_type==TRAC_ACTION_SET_TIME) || (act_type==TRAC_ACTION_SET_MEDIA_TIME)) {
		struct tm time;
		char *ext = strchr(arg_val, '=');
		if (ext) {
			ext[0] = 0;
			tka->trackID = atoi(arg_val);
			ext[0] = '=';
			arg_val = ext+1;
		}
		memset(&time, 0, sizeof(struct tm));
		sscanf(arg_val, "%d/%d/%d-%d:%d:%d", &time.tm_mday, &time.tm_mon, &time.tm_year, &time.tm_hour, &time.tm_min, &time.tm_sec);
		time.tm_isdst = 0;
		time.tm_year -= 1900;
		time.tm_mon -= 1;
		tka->time = 2082758400;
		tka->time += mktime(&time);
		return GF_TRUE;
	}

	while (param) {
		param = gf_url_colon_suffix(param);
		if (param) {
			*param = 0;
			param++;
#ifndef GPAC_DISABLE_MEDIA_EXPORT
			if (!strncmp("vttnomerge", param, 10)) {
				tka->dump_type |= GF_EXPORT_WEBVTT_NOMERGE;
			} else if (!strncmp("layer", param, 5)) {
				tka->dump_type |= GF_EXPORT_SVC_LAYER;
			} else if (!strncmp("full", param, 4)) {
				tka->dump_type |= GF_EXPORT_NHML_FULL;
			} else if (!strncmp("embedded", param, 8)) {
				tka->dump_type |= GF_EXPORT_WEBVTT_META_EMBEDDED;
			} else if (!strncmp("output=", param, 7)) {
				tka->out_name = gf_strdup(param+7);
			} else if (!strncmp("src=", param, 4)) {
				tka->src_name = gf_strdup(param+4);
			} else if (!strncmp("str=", param, 4)) {
				tka->string = gf_strdup(param+4);
			} else if (!strncmp("box=", param, 4)) {
				tka->src_name = gf_strdup(param+4);
				tka->sample_num = 1;
			} else if (!strncmp("type=", param, 4)) {
				tka->udta_type = GF_4CC(param[5], param[6], param[7], param[8]);
			} else if (tka->dump_type == GF_EXPORT_RAW_SAMPLES) {
				tka->sample_num = atoi(param);
			}
#endif
		}
	}
	if (arg_val) {
		if (!strcmp(arg_val, "*")) {
			tka->trackID = (u32) -1;
		} else {
			if (act_type==TRAC_ACTION_RAW_EXTRACT) {
				if (!strncmp(arg_val, "video", 5)) {
					arg_val += 5;
					tka->dump_track_type = 1;
				}
				else if (!strncmp(arg_val, "audio", 5)) {
					arg_val += 5;
					tka->dump_track_type = 2;
				}
			}
			if (arg_val[0])
				tka->trackID = atoi(arg_val);
		}
	}
	return GF_TRUE;
}