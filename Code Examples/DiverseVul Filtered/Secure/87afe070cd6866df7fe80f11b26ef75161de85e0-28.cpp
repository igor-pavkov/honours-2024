static GF_Err do_meta_act()
{
	u32 i;
	for (i=0; i<nb_meta_act; i++) {
		GF_Err e = GF_OK;
		u32 tk = 0;
#ifndef GPAC_DISABLE_ISOM_WRITE
		Bool self_ref;
#endif
		MetaAction *meta = &metas[i];

		if (meta->trackID) tk = gf_isom_get_track_by_id(file, meta->trackID);

		switch (meta->act_type) {
#ifndef GPAC_DISABLE_ISOM_WRITE
		case META_ACTION_SET_TYPE:
			/*note: we don't handle file brand modification, this is an author stuff and cannot be guessed from meta type*/
			e = gf_isom_set_meta_type(file, meta->root_meta, tk, meta->meta_4cc);
			gf_isom_modify_alternate_brand(file, GF_ISOM_BRAND_ISO2, GF_TRUE);
			do_save = GF_TRUE;
			break;
		case META_ACTION_ADD_ITEM:
			self_ref = !stricmp(meta->szPath, "NULL") || !stricmp(meta->szPath, "this") || !stricmp(meta->szPath, "self");
			e = gf_isom_add_meta_item(file, meta->root_meta, tk, self_ref, self_ref ? NULL : meta->szPath,
			                          meta->szName,
			                          meta->item_id,
									  meta->item_type,
			                          meta->mime_type,
			                          meta->enc_type,
			                          meta->use_dref ? meta->szPath : NULL,  NULL,
			                          meta->image_props);
			if (meta->item_refs && gf_list_count(meta->item_refs)) {
				u32 ref_i;
				for (ref_i = 0; ref_i < gf_list_count(meta->item_refs); ref_i++) {
					MetaRef	*ref_entry = gf_list_get(meta->item_refs, ref_i);
					e = gf_isom_meta_add_item_ref(file, meta->root_meta, tk, meta->item_id, ref_entry->ref_item_id, ref_entry->ref_type, NULL);
				}
			}
			do_save = GF_TRUE;
			break;
		case META_ACTION_ADD_IMAGE_ITEM:
		{
			u32 old_tk_count = gf_isom_get_track_count(file);
			u32 src_tk_id = 1;
			GF_Fraction _frac = {0,0};
			GF_ISOFile *fsrc = file;
			self_ref = GF_FALSE;

			tk = 0;
			if (meta->image_props && meta->image_props->auto_grid) {
				e = GF_OK;
				self_ref = GF_TRUE;
			} else if (!meta->szPath || (meta->image_props && meta->image_props->sample_num && meta->image_props->use_reference)) {
				e = GF_OK;
				self_ref = GF_TRUE;
				src_tk_id = meta->trackID;
			} else if (meta->szPath) {
				if (meta->image_props && gf_isom_probe_file(meta->szPath) && !meta->image_props->tile_mode) {
					meta->image_props->src_file = gf_isom_open(meta->szPath, GF_ISOM_OPEN_READ, NULL);
					e = gf_isom_last_error(meta->image_props->src_file);
					fsrc = meta->image_props->src_file;
				} else {
					e = import_file(file, meta->szPath, 0, _frac, 0, NULL, NULL, 0);
				}
			} else {
				M4_LOG(GF_LOG_ERROR, ("Missing file name to import\n"));
				e = GF_BAD_PARAM;
			}
			if (e == GF_OK) {
				u32 meta_type = gf_isom_get_meta_type(file, meta->root_meta, tk);
				if (!meta_type) {
					e = gf_isom_set_meta_type(file, meta->root_meta, tk, GF_META_ITEM_TYPE_PICT);
				} else {
					if (meta_type != GF_META_ITEM_TYPE_PICT) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("Warning: file already has a root 'meta' box of type %s\n", gf_4cc_to_str(meta_type)));
						e = GF_BAD_PARAM;
					}
				}
				if (e == GF_OK) {
					if (!meta->item_id) {
						e = gf_isom_meta_get_next_item_id(file, meta->root_meta, tk, &meta->item_id);
					}
					if (e == GF_OK) {
						if (!src_tk_id) {
							u32 j;
							for (j=0; j<gf_isom_get_track_count(fsrc); j++) {
								if (gf_isom_is_video_handler_type (gf_isom_get_media_type(fsrc, j+1))) {
									src_tk_id = gf_isom_get_track_id(fsrc, j+1);
									break;
								}
							}

							if (!src_tk_id) {
								M4_LOG(GF_LOG_ERROR, ("No video track in file, cannot add image from track\n"));
								e = GF_BAD_PARAM;
								break;
							}
						}

						e = gf_isom_iff_create_image_item_from_track(file, meta->root_meta, tk, src_tk_id, meta->szName, meta->item_id, meta->image_props, NULL);
						if (e == GF_OK && meta->primary) {
							e = gf_isom_set_meta_primary_item(file, meta->root_meta, tk, meta->item_id);
						}
						if (e == GF_OK && meta->item_refs && gf_list_count(meta->item_refs)) {
							u32 ref_i;
							for (ref_i = 0; ref_i < gf_list_count(meta->item_refs); ref_i++) {
								MetaRef	*ref_entry = gf_list_get(meta->item_refs, ref_i);
								e = gf_isom_meta_add_item_ref(file, meta->root_meta, tk, meta->item_id, ref_entry->ref_item_id, ref_entry->ref_type, NULL);
							}
						}
						if (e == GF_OK && meta->group_type) {
							e = gf_isom_meta_add_item_group(file, meta->root_meta, tk, meta->item_id, meta->group_id, meta->group_type);
						}
					}
				}
			}
			if (meta->image_props && meta->image_props->src_file) {
				gf_isom_delete(meta->image_props->src_file);
				meta->image_props->src_file = NULL;
			} else if (!self_ref) {
				gf_isom_remove_track(file, old_tk_count+1);
				if (do_flat) {
					M4_LOG(GF_LOG_ERROR, ("Warning: -flat storage cannot be used when using -add-image on external file\n"));
					e = GF_NOT_SUPPORTED;
				}
			}
			do_save = GF_TRUE;
		}
			break;
		case META_ACTION_ADD_IMAGE_GRID:
		{
			u32 meta_type = gf_isom_get_meta_type(file, meta->root_meta, tk);
			e = GF_OK;
			if (!meta_type) {
				e = gf_isom_set_meta_type(file, meta->root_meta, tk, GF_META_ITEM_TYPE_PICT);
			} else {
				if (meta_type != GF_META_ITEM_TYPE_PICT) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("Warning: file already has a root 'meta' box of type %s\n", gf_4cc_to_str(meta_type)));
					e = GF_BAD_PARAM;
				}
			}
			if (e == GF_OK) {
				if (!meta->item_id) {
					e = gf_isom_meta_get_next_item_id(file, meta->root_meta, tk, &meta->item_id);
				}
				if (e == GF_OK) {
					e = gf_isom_iff_create_image_grid_item(file, meta->root_meta, tk,
							meta->szName && strlen(meta->szName) ? meta->szName : NULL,
							meta->item_id,
							meta->image_props);
					if (e == GF_OK && meta->primary) {
						e = gf_isom_set_meta_primary_item(file, meta->root_meta, tk, meta->item_id);
					}
					if (e == GF_OK && meta->item_refs && gf_list_count(meta->item_refs)) {
						u32 ref_i;
						for (ref_i = 0; ref_i < gf_list_count(meta->item_refs); ref_i++) {
							MetaRef	*ref_entry = gf_list_get(meta->item_refs, ref_i);
							e = gf_isom_meta_add_item_ref(file, meta->root_meta, tk, meta->item_id, ref_entry->ref_item_id, ref_entry->ref_type, NULL);
						}
					}
				}
			}
			do_save = GF_TRUE;
		}
			break;
		case META_ACTION_REM_ITEM:
			e = gf_isom_remove_meta_item(file, meta->root_meta, tk, meta->item_id);
			do_save = GF_TRUE;
			break;
		case META_ACTION_SET_PRIMARY_ITEM:
			e = gf_isom_set_meta_primary_item(file, meta->root_meta, tk, meta->item_id);
			do_save = GF_TRUE;
			break;
		case META_ACTION_SET_XML:
		case META_ACTION_SET_BINARY_XML:
			e = gf_isom_set_meta_xml(file, meta->root_meta, tk, meta->szPath, NULL, 0, (meta->act_type==META_ACTION_SET_BINARY_XML) ? 1 : 0);
			do_save = GF_TRUE;
			break;
		case META_ACTION_REM_XML:
			if (gf_isom_get_meta_item_count(file, meta->root_meta, tk)) {
				e = gf_isom_remove_meta_xml(file, meta->root_meta, tk);
				do_save = GF_TRUE;
			} else {
				M4_LOG(GF_LOG_WARNING, ("No meta box in input file\n"));
				e = GF_OK;
			}
			break;
		case META_ACTION_DUMP_ITEM:
			if (gf_isom_get_meta_item_count(file, meta->root_meta, tk)) {
				e = gf_isom_extract_meta_item(file, meta->root_meta, tk, meta->item_id, meta->szPath && strlen(meta->szPath) ? meta->szPath : NULL);
			} else {
				M4_LOG(GF_LOG_WARNING, ("No meta box in input file\n"));
				e = GF_OK;
			}
			break;
#endif // GPAC_DISABLE_ISOM_WRITE

		case META_ACTION_DUMP_XML:
			if (gf_isom_has_meta_xml(file, meta->root_meta, tk)) {
				e = gf_isom_extract_meta_xml(file, meta->root_meta, tk, meta->szPath, NULL);
			} else {
				M4_LOG(GF_LOG_WARNING, ("No meta box in input file\n"));
				e = GF_OK;
			}
			break;
		default:
			break;
		}
		if (meta->item_refs) {
			while (gf_list_count(meta->item_refs)) {
				gf_free(gf_list_pop_back(meta->item_refs));
			}
			gf_list_del(meta->item_refs);
			meta->item_refs = NULL;
		}
		if (meta->image_props) {
			gf_free(meta->image_props);
			meta->image_props = NULL;
		}
		if (e) return e;
	}
	return GF_OK;
}