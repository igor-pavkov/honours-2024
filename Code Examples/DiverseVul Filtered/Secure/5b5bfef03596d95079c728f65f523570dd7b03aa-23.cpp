void mainwindow_set_size(MAIN_WINDOW_REC *window, int height, int resize_lower)
{
        height -= window->height;
	if (height < 0)
		mainwindow_shrink(window, -height, resize_lower);
	else
		mainwindow_grow(window, height, resize_lower);
}