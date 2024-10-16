static u32 parse_meta_args(char *opts, MetaActionType act_type)
{
	MetaAction *meta;

	metas = gf_realloc(metas, sizeof(MetaAction) * (nb_meta_act + 1));
	if (!metas) return 2;
	meta = &metas[nb_meta_act];
	nb_meta_act ++;

	memset(meta, 0, sizeof(MetaAction));
	meta->act_type = act_type;
	meta->trackID = 0;
	meta->root_meta = 1;
	open_edit = GF_TRUE;

	if (!opts) return 2;

	if (act_type == META_ACTION_ADD_IMAGE_ITEM)
		has_add_image = GF_TRUE;

	while (1) {
		char *next;
		char *szSlot;
		if (!opts || !opts[0]) return 0;
		if (opts[0]==':') opts += 1;

		szSlot = opts;
		next = gf_url_colon_suffix(opts);
		if (next) next[0] = 0;

		if (!strnicmp(szSlot, "tk=", 3)) {
			sscanf(szSlot, "tk=%u", &meta->trackID);
			meta->root_meta = 0;
		}
		else if (!strnicmp(szSlot, "id=", 3)) {
			meta->item_id = atoi(szSlot+3);
		}
		else if (!strnicmp(szSlot, "type=", 5)) {
			meta->item_type = GF_4CC(szSlot[5], szSlot[6], szSlot[7], szSlot[8]);
		}
		//"ref" (without '=') is for data reference, "ref=" is for item references
		else if (!strnicmp(szSlot, "ref=", 4)) {
			char type[5];
			MetaRef	*ref;
			if (!meta->item_refs) {
				meta->item_refs = gf_list_new();
				if (!meta->item_refs) return 2;
			}
			GF_SAFEALLOC(ref, MetaRef);
			if (!ref) return 2;
			sscanf(szSlot, "ref=%4s,%u", type, &(ref->ref_item_id));
			ref->ref_type = GF_4CC(type[0], type[1], type[2], type[3]);
			gf_list_add(meta->item_refs, ref);
		}
		else if (!strnicmp(szSlot, "name=", 5)) {
			meta->szName = gf_strdup(szSlot+5);
		}
		else if (!strnicmp(szSlot, "path=", 5)) {
			meta->szPath = gf_strdup(szSlot+5);
		}
		else if (!strnicmp(szSlot, "mime=", 5)) {
			meta->item_type = GF_META_ITEM_TYPE_MIME;
			meta->mime_type = gf_strdup(szSlot+5);
		}
		else if (!strnicmp(szSlot, "encoding=", 9)) {
			meta->enc_type = gf_strdup(szSlot+9);
		}
		else if (!strnicmp(szSlot, "image-size=", 11)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			sscanf(szSlot+11, "%dx%d", &meta->image_props->width, &meta->image_props->height);
		}
		else if (!strnicmp(szSlot, "image-grid-size=", 16)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
			}
			sscanf(szSlot+16, "%dx%d", &meta->image_props->num_grid_rows, &meta->image_props->num_grid_columns);
		}
		else if (!strnicmp(szSlot, "image-pasp=", 11)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			sscanf(szSlot+11, "%dx%d", &meta->image_props->hSpacing, &meta->image_props->vSpacing);
		}
		else if (!strnicmp(szSlot, "image-rloc=", 11)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			sscanf(szSlot+11, "%dx%d", &meta->image_props->hOffset, &meta->image_props->vOffset);
		}
		else if (!strnicmp(szSlot, "rotation=", 9)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			meta->image_props->angle = atoi(szSlot+9);
		}
		else if (!strnicmp(szSlot, "mirror-axis=", 12)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			meta->image_props->mirror = (!strnicmp(szSlot+12, "vertical", 8) ? 1 : 2);
		}
		else if (!strnicmp(szSlot, "clap=", 5)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			sscanf(szSlot + 5, "%d,%d,%d,%d,%d,%d,%d,%d", &meta->image_props->clap_wnum, &meta->image_props->clap_wden,
					   &meta->image_props->clap_hnum, &meta->image_props->clap_hden,
					   &meta->image_props->clap_honum, &meta->image_props->clap_hoden,
					   &meta->image_props->clap_vonum, &meta->image_props->clap_voden);
		}
		else if (!stricmp(szSlot, "hidden")) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			meta->image_props->hidden = GF_TRUE;
		}
		else if (!stricmp(szSlot, "alpha")) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			meta->image_props->alpha = GF_TRUE;
		}
		//"ref" (without '=') is for data reference, "ref=" is for item references
		else if (!stricmp(szSlot, "ref")) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			meta->image_props->use_reference = GF_TRUE;
		}
		else if (!strnicmp(szSlot, "time=", 5)) {
			Float s=0, e=0, step=0;
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			if (sscanf(szSlot+5, "%f-%f/%f", &s, &e, &step)==3) {
				meta->image_props->time = s;
				meta->image_props->end_time = e;
				meta->image_props->step_time = step;
			} else if (sscanf(szSlot+5, "%f-%f", &s, &e)==2) {
				meta->image_props->time = s;
				meta->image_props->end_time = e;
			} else if (sscanf(szSlot+5, "%f/%f", &s, &step)==2) {
				meta->image_props->time = s;
				meta->image_props->step_time = step;
			} else if (sscanf(szSlot+5, "%f", &s)==1) {
				meta->image_props->time = s;
			}
		}
		else if (!strnicmp(szSlot, "samp=", 5)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			meta->image_props->sample_num = atoi(szSlot+5);
			meta->root_meta = 1;
		}
		else if (!strnicmp(szSlot, "group=", 6)) {
			char type[5];
			sscanf(szSlot, "group=%4s,%u", type, &meta->group_id);
			meta->group_type = GF_4CC(type[0], type[1], type[2], type[3]);
		}
		else if (!stricmp(szSlot, "split_tiles")) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			meta->image_props->tile_mode = TILE_ITEM_ALL_BASE;
		}
		else if (!stricmp(szSlot, "dref")) {
			meta->use_dref = 1;
		}
		else if (!stricmp(szSlot, "primary")) {
			meta->primary = 1;
		}
		else if (!stricmp(szSlot, "binary")) {
			if (meta->act_type==META_ACTION_SET_XML) meta->act_type=META_ACTION_SET_BINARY_XML;
		}
		else if (!strnicmp(szSlot, "icc_path=", 9)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			strcpy(meta->image_props->iccPath, szSlot+9);
		}
		else if (!stricmp(szSlot, "agrid") || !strnicmp(szSlot, "agrid=", 6)) {
			if (!meta->image_props) {
				GF_SAFEALLOC(meta->image_props, GF_ImageItemProperties);
				if (!meta->image_props) return 2;
			}
			meta->image_props->auto_grid = GF_TRUE;
			if (!strnicmp(szSlot, "agrid=", 6))
				meta->image_props->auto_grid_ratio = atof(szSlot+6);
		}
		else if (!strchr(szSlot, '=')) {
			switch (meta->act_type) {
			case META_ACTION_SET_TYPE:
				if (!stricmp(szSlot, "null") || !stricmp(szSlot, "0")) meta->meta_4cc = 0;
				else meta->meta_4cc = GF_4CC(szSlot[0], szSlot[1], szSlot[2], szSlot[3]);
				break;
			case META_ACTION_ADD_ITEM:
			case META_ACTION_ADD_IMAGE_ITEM:
			case META_ACTION_SET_XML:
			case META_ACTION_DUMP_XML:
				if (!strncmp(szSlot, "dopt", 4) || !strncmp(szSlot, "sopt", 4) || !strncmp(szSlot, "@", 1)) {
					if (next) next[0]=':';
					next=NULL;
				}
				//cat as -add arg
				gf_dynstrcat(&meta->szPath, szSlot, ":");
				if (!meta->szPath) return 2;
				break;
			case META_ACTION_REM_ITEM:
			case META_ACTION_SET_PRIMARY_ITEM:
			case META_ACTION_DUMP_ITEM:
				meta->item_id = atoi(szSlot);
				break;
			default:
				break;
			}
		}
		if (!next) break;
		opts += strlen(szSlot);
		next[0] = ':';
	}
	return 0;
}