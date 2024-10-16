void open_files(GSList *filenames)
{
	GSList *fns = NULL;		/* File names to ask */
	GSList *fns_is_mod = NULL;	/* File name layer is modified */
	GSList *fns_cnt = NULL;		/* File names count */
	GSList *fns_lay_num = NULL;	/* Layer number for fns */
	GSList *cnt = NULL;		/* File names count unsorted by layers,
					   0 -- file not yet loaded as layer */
	gint answer;

	if (filenames == NULL)
		return;

	/* Check if there is a Gerbv project in the list.
	 * If there is least open only that and ignore the rest. */
	for (GSList *fn = filenames; fn; fn = fn->next) {
		gboolean is_project = FALSE;
		if (0 == project_is_gerbv_project(fn->data, &is_project)
		&&  is_project) {
			open_project(fn->data);

			gerbv_render_zoom_to_fit_display(mainProject,
					&screenRenderInfo);
			render_refresh_rendered_image_on_screen();
			callbacks_update_layer_tree();

			return;
		}
	}

	/* Count opened filenames and place result in list */
	for (GSList *fn = filenames; fn; fn = fn->next) {
		gint c = 0;

		for (gint fidx = 0; fidx <= mainProject->last_loaded; ++fidx) {
			gchar *fpn = mainProject->file[fidx]->fullPathname;

			if (strlen(fpn) == strlen(fn->data)
			&&  0 == g_ascii_strncasecmp(fpn, fn->data,
						strlen(fn->data))) {
				c++;
			}
		}

		cnt = g_slist_append(cnt, GINT_TO_POINTER(c));
	}

	/* Make fns, fns_is_mod and fns_cnt lists sorted by layers */
	for (gint fidx = 0; fidx <= mainProject->last_loaded; ++fidx) {
		gchar *fpn = mainProject->file[fidx]->fullPathname;

		for (GSList *fn = filenames; fn; fn = fn->next) {
			if (strlen(fpn) == strlen(fn->data)
			&&  0 == g_ascii_strncasecmp(fpn, fn->data,
					strlen(fn->data))) {
				fns = g_slist_append(fns, fn->data);
				fns_is_mod = g_slist_append(fns_is_mod,
						GINT_TO_POINTER(mainProject->
							file[fidx]->
							layer_dirty));
				fns_cnt = g_slist_append(fns_cnt,
						g_slist_nth_data(cnt,
							g_slist_position(
								filenames,
								fn)));
				fns_lay_num = g_slist_append(fns_lay_num,
						GINT_TO_POINTER(fidx));

				break;
			}
		}
	}

	answer = GTK_RESPONSE_NONE;
	if (g_slist_length(fns) > 0)
		answer = interface_reopen_question(fns, fns_is_mod,
							fns_cnt, fns_lay_num);

	switch (answer) {

	case GTK_RESPONSE_CANCEL:
	case GTK_RESPONSE_NONE:
	case GTK_RESPONSE_DELETE_EVENT:
		/* Dialog is closed or Esc is pressed, skip all */
		break;

	case GTK_RESPONSE_YES: /* Reload layer was selected */
		for (GSList *fn = fns; fn; fn = fn->next) {
			if (fn->data != NULL)
				gerbv_revert_file(mainProject,
					GPOINTER_TO_INT(
						g_slist_nth_data (fns_lay_num,
							g_slist_position (fns,
									fn))));
		}
		break;

	case GTK_RESPONSE_OK: /* Open as a new layer was selected */
		/* To open as new only _one_ instance of file, check filenames
		 * by selected files in fns */
		for (GSList *fn = filenames; fn; fn = fn->next) {
			if (NULL != g_slist_find (fns, fn->data))
				gerbv_open_layer_from_filename(mainProject,
								fn->data);
		}
		break;
	}

	/* Add not loaded files (cnt == 0) in the end */
	for (GSList *fn = filenames; fn; fn = fn->next) {
		if (0 == GPOINTER_TO_INT(g_slist_nth_data(cnt,
					g_slist_position(filenames, fn))))
			gerbv_open_layer_from_filename (mainProject, fn->data);
	}

	g_slist_free(fns);
	g_slist_free(fns_is_mod);
	g_slist_free(fns_cnt);
	g_slist_free(fns_lay_num);
	g_slist_free(cnt);

	gerbv_render_zoom_to_fit_display (mainProject, &screenRenderInfo);
	render_refresh_rendered_image_on_screen();
	callbacks_update_layer_tree();
}