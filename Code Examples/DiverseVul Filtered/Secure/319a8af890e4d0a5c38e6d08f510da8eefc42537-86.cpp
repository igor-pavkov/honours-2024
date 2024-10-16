void open_project(char *project_filename)
{

/* TODO: check if layers is modified and show it to user. */

	if (mainProject->last_loaded >= 0
	&&  !interface_get_alert_dialog_response (
		_("Do you want to close any open layers and load "
		"an existing project?"),
		_("Loading a project will cause all currently open "
		"layers to be closed. Any unsaved changes "
		"will be lost."),
		FALSE, NULL, GTK_STOCK_CLOSE, GTK_STOCK_CANCEL)) {

			return;
	}

	/* Update the last folder */
	g_free (mainProject->path);
	mainProject->path = g_strdup(project_filename);

	gerbv_unload_all_layers (mainProject);
	main_open_project_from_filename (mainProject, project_filename);
}