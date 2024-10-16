static GF_Err do_track_act()
{
	u32 j;
	for (j=0; j<nb_track_act; j++) {
		u32 i;
		GF_Err e = GF_OK;
		TrackAction *tka = &tracks[j];
		u32 track = tka->trackID ? gf_isom_get_track_by_id(file, tka->trackID) : 0;

		timescale = gf_isom_get_timescale(file);
		switch (tka->act_type) {
		case TRAC_ACTION_REM_TRACK:
			e = gf_isom_remove_track(file, track);
			if (e) {
				M4_LOG(GF_LOG_ERROR, ("Error Removing track ID %d: %s\n", tka->trackID, gf_error_to_string(e)));
			} else {
				M4_LOG(GF_LOG_INFO, ("Removing track ID %d\n", tka->trackID));
			}
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_SET_LANGUAGE:
			for (i=0; i<gf_isom_get_track_count(file); i++) {
				if (track && (track != i+1)) continue;
				e = gf_isom_set_media_language(file, i+1, tka->lang);
				if (e) return e;
				do_save = GF_TRUE;
			}
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_SET_KIND:
			for (i=0; i<gf_isom_get_track_count(file); i++) {
				if (track && (track != i+1)) continue;
				e = gf_isom_add_track_kind(file, i+1, tka->kind_scheme, tka->kind_value);
				if (e) return e;
				do_save = GF_TRUE;
			}
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_REM_KIND:
			for (i=0; i<gf_isom_get_track_count(file); i++) {
				if (track && (track != i+1)) continue;
				e = gf_isom_remove_track_kind(file, i+1, tka->kind_scheme, tka->kind_value);
				if (e) return e;
				do_save = GF_TRUE;
			}
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_SET_DELAY:
			if (tka->delay.num && tka->delay.den) {
				u64 tk_dur;

				e = gf_isom_remove_edits(file, track);
				if (e) return e;
				tk_dur = gf_isom_get_track_duration(file, track);
				if (gf_isom_get_edits_count(file, track))
					do_save = GF_TRUE;
				if (tka->delay.num>0) {
					//cast to u64, delay_ms * timescale can be quite big before / 1000
					e = gf_isom_append_edit(file, track, ((u64) tka->delay.num) * timescale / tka->delay.den, 0, GF_ISOM_EDIT_EMPTY);
					if (e) return e;
					e = gf_isom_append_edit(file, track, tk_dur, 0, GF_ISOM_EDIT_NORMAL);
					if (e) return e;
					do_save = GF_TRUE;
				} else {
					//cast to u64, delay_ms * timescale can be quite big before / 1000
					u64 to_skip = ((u64) -tka->delay.num) * timescale / tka->delay.den;
					if (to_skip<tk_dur) {
						//cast to u64, delay_ms * timescale can be quite big before / 1000
						u64 media_time = ((u64) -tka->delay.num) * gf_isom_get_media_timescale(file, track) / tka->delay.den;
						e = gf_isom_append_edit(file, track, tk_dur-to_skip, media_time, GF_ISOM_EDIT_NORMAL);
						if (e) return e;
						do_save = GF_TRUE;
					} else {
						M4_LOG(GF_LOG_WARNING, ("Warning: request negative delay longer than track duration - ignoring\n"));
					}
				}
			} else if (gf_isom_get_edits_count(file, track)) {
				e = gf_isom_remove_edits(file, track);
				if (e) return e;
				do_save = GF_TRUE;
			}
			break;
		case TRAC_ACTION_SET_KMS_URI:
			for (i=0; i<gf_isom_get_track_count(file); i++) {
				if (track && (track != i+1)) continue;
				if (!gf_isom_is_media_encrypted(file, i+1, 1)) continue;
				if (!gf_isom_is_ismacryp_media(file, i+1, 1)) continue;
				e = gf_isom_change_ismacryp_protection(file, i+1, 1, NULL, (char *) tka->kms);
				if (e) return e;
				do_save = GF_TRUE;
			}
			break;
		case TRAC_ACTION_SET_ID:
			if (!tka->trackID && (gf_isom_get_track_count(file) == 1)) {
				M4_LOG(GF_LOG_WARNING, ("Warning: track id is not specified, but file has only one track - assume that you want to change id for this track\n"));
				track = 1;
			}
			if (track) {
				u32 newTrack;
				newTrack = gf_isom_get_track_by_id(file, tka->newTrackID);
				if (newTrack != 0) {
					M4_LOG(GF_LOG_WARNING, ("Cannot set track id with value %d because a track already exists - ignoring", tka->newTrackID));
				} else {
					e = gf_isom_set_track_id(file, track, tka->newTrackID);
					if (e) return e;
					do_save = GF_TRUE;
				}
			} else {
				M4_LOG(GF_LOG_WARNING, ("Error: Cannot change id for track %d because it does not exist - ignoring", tka->trackID));
			}
			break;
		case TRAC_ACTION_SWAP_ID:
			if (track) {
				u32 tk1, tk2;
				tk1 = gf_isom_get_track_by_id(file, tka->trackID);
				tk2 = gf_isom_get_track_by_id(file, tka->newTrackID);
				if (!tk1 || !tk2) {
					M4_LOG(GF_LOG_WARNING, ("Error: Cannot swap track IDs because not existing - ignoring"));
				} else {
					e = gf_isom_set_track_id(file, tk2, 0);
					if (e) return e;
					e = gf_isom_set_track_id(file, tk1, tka->newTrackID);
					if (e) return e;
					e = gf_isom_set_track_id(file, tk2, tka->trackID);
					if (e) return e;
					do_save = GF_TRUE;
				}
			} else {
				M4_LOG(GF_LOG_WARNING, ("Error: Cannot change id for track %d because it does not exist - ignoring", tka->trackID));
			}
			break;
		case TRAC_ACTION_SET_PAR:
			e = gf_media_change_par(file, track, tka->par_num, tka->par_den, tka->force_par, tka->rewrite_bs);
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_SET_CLAP:
			e = gf_isom_set_clean_aperture(file, track, 1, tka->clap_wnum, tka->clap_wden, tka->clap_hnum, tka->clap_hden, tka->clap_honum, tka->clap_hoden, tka->clap_vonum, tka->clap_voden);
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_SET_MX:
			e = gf_isom_set_track_matrix(file, track, tka->mx);
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_SET_HANDLER_NAME:
			e = gf_isom_set_handler_name(file, track, tka->hdl_name);
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_ENABLE:
			if (!gf_isom_is_track_enabled(file, track)) {
				e = gf_isom_set_track_enabled(file, track, GF_TRUE);
				do_save = GF_TRUE;
			}
			break;
		case TRAC_ACTION_DISABLE:
			if (gf_isom_is_track_enabled(file, track)) {
				e = gf_isom_set_track_enabled(file, track, GF_FALSE);
				do_save = GF_TRUE;
			}
			break;
		case TRAC_ACTION_REFERENCE:
			e = gf_isom_set_track_reference(file, track, GF_4CC(tka->lang[0], tka->lang[1], tka->lang[2], tka->lang[3]), tka->newTrackID);
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_REM_NON_RAP:
			e = gf_media_remove_non_rap(file, track, GF_FALSE);
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_REM_NON_REFS:
			e = gf_media_remove_non_rap(file, track, GF_TRUE);
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_SET_UDTA:
			e = set_file_udta(file, track, tka->udta_type, tka->string ? tka->string : tka->src_name , tka->sample_num ? GF_TRUE : GF_FALSE, tka->string ? GF_TRUE : GF_FALSE);
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_SET_EDITS:
			e = apply_edits(file, track, tka->string);
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_SET_TIME:
			if (!tka->trackID) {
				e = gf_isom_set_creation_time(file, tka->time, tka->time);
				if (e) return e;
				for (i=0; i<gf_isom_get_track_count(file); i++) {
					e = gf_isom_set_track_creation_time(file, i+1, tka->time, tka->time);
					if (e) return e;
				}
			} else {
				e = gf_isom_set_track_creation_time(file, track, tka->time, tka->time);
			}
			do_save = GF_TRUE;
			break;
		case TRAC_ACTION_SET_MEDIA_TIME:
			for (i=0; i<gf_isom_get_track_count(file); i++) {
				if (track && (track != i+1)) continue;
				e = gf_isom_set_media_creation_time(file, i+1, tka->time, tka->time);
				if (e) return e;
			}
			do_save = GF_TRUE;
			break;
		default:
			break;
		}
		if (e) return e;
	}
	return GF_OK;
}