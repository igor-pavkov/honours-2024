void callbacks_hadjustment_value_changed (GtkAdjustment *adjustment, gpointer user_data){
	/* make sure we're actually using the scrollbar to make sure we don't reset
	   lowerLeftX during a scrollbar redraw during something else */
	if (screen.state == SCROLLBAR) {
		screenRenderInfo.lowerLeftX = gtk_adjustment_get_value (adjustment);
	}
}