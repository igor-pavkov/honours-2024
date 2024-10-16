GF_Err dump_isom_scene(char *file, char *inName, Bool is_final_name, GF_SceneDumpFormat dump_mode, Bool do_log, Bool no_odf_conv)
{
	GF_Err e;
	GF_SceneManager *ctx;
	GF_SceneGraph *sg;
	GF_SceneLoader load;
	GF_FileType ftype;
	gf_log_cbk prev_logs = NULL;
	FILE *logs = NULL;

	sg = gf_sg_new();
	ctx = gf_sm_new(sg);
	memset(&load, 0, sizeof(GF_SceneLoader));
	load.fileName = file;
	load.ctx = ctx;
	load.swf_import_flags = swf_flags;
	if (dump_mode == GF_SM_DUMP_SVG) {
		load.swf_import_flags |= GF_SM_SWF_USE_SVG;
		load.svgOutFile = inName;
	}
	load.swf_flatten_limit = swf_flatten_angle;

	ftype = get_file_type_by_ext(file);
	if (ftype == GF_FILE_TYPE_ISO_MEDIA) {
		load.isom = gf_isom_open(file, GF_ISOM_OPEN_READ, NULL);
		if (!load.isom) {
			e = gf_isom_last_error(NULL);
			fprintf(stderr, "Error opening file: %s\n", gf_error_to_string(e));
			gf_sm_del(ctx);
			gf_sg_del(sg);
			return e;
		}
		if (no_odf_conv)
			gf_isom_disable_odf_conversion(load.isom, GF_TRUE);

	} else if (ftype==GF_FILE_TYPE_LSR_SAF) {
		load.isom = gf_isom_open("saf_conv", GF_ISOM_WRITE_EDIT, NULL);
#ifndef GPAC_DISABLE_MEDIA_IMPORT
		if (load.isom) {
			GF_Fraction _frac = {0,0};
			e = import_file(load.isom, file, 0, _frac, 0, NULL, NULL, 0);
		} else
#else
		fprintf(stderr, "Warning: GPAC was compiled without Media Import support\n");
#endif
			e = gf_isom_last_error(NULL);

		if (e) {
			fprintf(stderr, "Error importing file: %s\n", gf_error_to_string(e));
			gf_sm_del(ctx);
			gf_sg_del(sg);
			if (load.isom) gf_isom_delete(load.isom);
			return e;
		}
	}

	if (do_log) {
		char szLog[GF_MAX_PATH];
		sprintf(szLog, "%s_dec.logs", inName);
		logs = gf_fopen(szLog, "wt");

		gf_log_set_tool_level(GF_LOG_CODING, GF_LOG_DEBUG);
		prev_logs = gf_log_set_callback(logs, scene_coding_log);
	}
	e = gf_sm_load_init(&load);
	if (!e) e = gf_sm_load_run(&load);
	gf_sm_load_done(&load);
	if (logs) {
		gf_log_set_tool_level(GF_LOG_CODING, GF_LOG_ERROR);
		gf_log_set_callback(NULL, prev_logs);
		gf_fclose(logs);
	}
	if (!e && dump_mode != GF_SM_DUMP_SVG) {
		u32 count = gf_list_count(ctx->streams);
		if (count)
			fprintf(stderr, "Scene loaded - dumping %d systems streams\n", count);
		else
			fprintf(stderr, "Scene loaded - dumping root scene\n");

		e = gf_sm_dump(ctx, inName, is_final_name, dump_mode);
	}

	gf_sm_del(ctx);
	gf_sg_del(sg);
	if (e) fprintf(stderr, "Error loading scene: %s\n", gf_error_to_string(e));
	if (load.isom) gf_isom_delete(load.isom);
	return e;
}