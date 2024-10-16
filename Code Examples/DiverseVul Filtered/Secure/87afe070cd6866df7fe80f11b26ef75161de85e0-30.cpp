static GF_Err do_tsel_act()
{
	u32 i;
	GF_Err e;
	for (i=0; i<nb_tsel_acts; i++) {
		switch (tsel_acts[i].act_type) {
		case TSEL_ACTION_SET_PARAM:
			e = gf_isom_set_track_switch_parameter(file,
			                                       gf_isom_get_track_by_id(file, tsel_acts[i].trackID),
			                                       tsel_acts[i].refTrackID ? gf_isom_get_track_by_id(file, tsel_acts[i].refTrackID) : 0,
			                                       tsel_acts[i].is_switchGroup ? 1 : 0,
			                                       &tsel_acts[i].switchGroupID,
			                                       tsel_acts[i].criteria, tsel_acts[i].nb_criteria);
			if (e == GF_BAD_PARAM) {
				u32 alternateGroupID, nb_groups;
				gf_isom_get_track_switch_group_count(file, gf_isom_get_track_by_id(file, tsel_acts[i].trackID), &alternateGroupID, &nb_groups);
				if (alternateGroupID) {
					M4_LOG(GF_LOG_ERROR, ("Error - for adding more tracks to group, using: -group-add -refTrack=ID1:[criteria:]trackID=ID2\n"));
				} else {
					M4_LOG(GF_LOG_ERROR, ("Error - for creating a new grouping information, using -group-add -trackID=ID1:[criteria:]trackID=ID2\n"));
				}
			}
			if (e) return e;
			do_save = GF_TRUE;
			break;
		case TSEL_ACTION_REMOVE_TSEL:
			e = gf_isom_reset_track_switch_parameter(file, gf_isom_get_track_by_id(file, tsel_acts[i].trackID), 0);
			if (e) return e;
			do_save = GF_TRUE;
			break;
		case TSEL_ACTION_REMOVE_ALL_TSEL_IN_GROUP:
			e = gf_isom_reset_track_switch_parameter(file, gf_isom_get_track_by_id(file, tsel_acts[i].trackID), 1);
			if (e) return e;
			do_save = GF_TRUE;
			break;
		default:
			break;
		}
	}
	return GF_OK;
}