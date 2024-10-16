static Bool parse_tsel_args(char *opts, TSELActionType act)
{
	GF_ISOTrackID refTrackID = 0;
	Bool has_switch_id;
	u32 switch_id = 0;
	u32 criteria[30];
	u32 nb_criteria = 0;
	TSELAction *tsel_act;
	char szSlot[1024];

	has_switch_id = 0;

	if (!opts) return 0;
	while (1) {
		char *next;
		if (!opts || !opts[0]) return 0;
		if (opts[0]==':') opts += 1;
		strcpy(szSlot, opts);
		next = gf_url_colon_suffix(szSlot);
		if (next) next[0] = 0;


		if (!strnicmp(szSlot, "refTrack=", 9)) refTrackID = atoi(szSlot+9);
		else if (!strnicmp(szSlot, "switchID=", 9)) {
			if (atoi(szSlot+9)<0) {
				switch_id = 0;
				has_switch_id = 0;
			} else {
				switch_id = atoi(szSlot+9);
				has_switch_id = 1;
			}
		}
		else if (!strnicmp(szSlot, "switchID", 8)) {
			switch_id = 0;
			has_switch_id = 1;
		}
		else if (!strnicmp(szSlot, "criteria=", 9)) {
			u32 j=9;
			nb_criteria = 0;
			while (j+3<strlen(szSlot)) {
				criteria[nb_criteria] = GF_4CC(szSlot[j], szSlot[j+1], szSlot[j+2], szSlot[j+3]);
				j+=5;
				nb_criteria++;
			}
		}
		else if (!strnicmp(szSlot, "trackID=", 8) || !strchr(szSlot, '=') ) {
			tsel_acts = gf_realloc(tsel_acts, sizeof(TSELAction) * (nb_tsel_acts + 1));
			if (!tsel_acts) return 2;

			tsel_act = &tsel_acts[nb_tsel_acts];
			nb_tsel_acts++;

			memset(tsel_act, 0, sizeof(TSELAction));
			tsel_act->act_type = act;
			tsel_act->trackID = strchr(szSlot, '=') ? atoi(szSlot+8) : atoi(szSlot);
			tsel_act->refTrackID = refTrackID;
			tsel_act->switchGroupID = switch_id;
			tsel_act->is_switchGroup = has_switch_id;
			tsel_act->nb_criteria = nb_criteria;
			memcpy(tsel_act->criteria, criteria, sizeof(u32)*nb_criteria);

			if (!refTrackID)
				refTrackID = tsel_act->trackID;

			open_edit = GF_TRUE;
		}
		opts += strlen(szSlot);
	}
	return 0;
}