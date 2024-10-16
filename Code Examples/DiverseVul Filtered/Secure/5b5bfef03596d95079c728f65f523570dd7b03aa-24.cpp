void mainwindow_destroy_half(MAIN_WINDOW_REC *window)
{
	int really_quitting = quitting;
	quitting = TRUE;
	mainwindow_destroy(window);
	quitting = really_quitting;
}