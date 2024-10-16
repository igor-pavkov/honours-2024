void callbacks_vadjustment_value_changed (GtkAdjustment *adjustment, gpointer user_data){
	/* make sure we're actually using the scrollbar to make sure we don't reset
	   lowerLeftY during a scrollbar redraw during something else */
	if (screen.state == SCROLLBAR) {
		screenRenderInfo.lowerLeftY = adjustment->upper -
			(gtk_adjustment_get_value (adjustment) + adjustment->page_size) + adjustment->lower;
	}
}