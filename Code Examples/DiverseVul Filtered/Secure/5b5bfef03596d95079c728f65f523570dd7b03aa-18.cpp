void mainwindow_set_rsize(MAIN_WINDOW_REC *window, int width)
{
	width -= window->width;
	if (width < 0)
		mainwindow_rshrink(window, -width);
	else
		mainwindow_rgrow(window, width);
}