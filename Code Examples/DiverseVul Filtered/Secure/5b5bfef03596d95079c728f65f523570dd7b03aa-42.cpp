static void mainwindow_resize(MAIN_WINDOW_REC *window, int xdiff, int ydiff)
{
	int height, width;
	if (quitting || (xdiff == 0 && ydiff == 0))
		return;

	height = window->height + ydiff;
	width = window->width + xdiff;
	window->width = window->last_column-window->first_column+1;
	window->height = window->last_line-window->first_line+1;
	if (height != window->height || width != window->width) {
		g_warning("Resizing window %p W:%d expected:%d H:%d expected:%d",
			  window, window->width, width, window->height, height);
	}
	window->size_dirty = TRUE;
}