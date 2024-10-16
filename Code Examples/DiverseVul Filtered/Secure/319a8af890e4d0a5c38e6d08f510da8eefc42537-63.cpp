void callbacks_force_expose_event_for_screen (void)
{
	GdkRectangle update_rect;
	
	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = screenRenderInfo.displayWidth;
	update_rect.height = screenRenderInfo.displayHeight;

	/* Calls expose_event */
	gdk_window_invalidate_rect (screen.drawing_area->window, &update_rect, FALSE);
	
	/* update other gui things that could have changed */
	callbacks_update_ruler_scales ();
	callbacks_update_scrollbar_limits ();
	callbacks_update_scrollbar_positions ();
}