void callbacks_update_scrollbar_positions (void){
	gdouble positionX,positionY;

	positionX = screenRenderInfo.lowerLeftX;
	if (positionX < ((GtkAdjustment *)screen.win.hAdjustment)->lower)
		positionX = ((GtkAdjustment *)screen.win.hAdjustment)->lower;
	if (positionX > (((GtkAdjustment *)screen.win.hAdjustment)->upper - ((GtkAdjustment *)screen.win.hAdjustment)->page_size))
		positionX = (((GtkAdjustment *)screen.win.hAdjustment)->upper - ((GtkAdjustment *)screen.win.hAdjustment)->page_size);
	gtk_adjustment_set_value ((GtkAdjustment *)screen.win.hAdjustment, positionX);
	
	positionY = ((GtkAdjustment *)screen.win.vAdjustment)->upper - screenRenderInfo.lowerLeftY -
		((GtkAdjustment *)screen.win.vAdjustment)->page_size +
		((GtkAdjustment *)screen.win.vAdjustment)->lower;
	if (positionY < ((GtkAdjustment *)screen.win.vAdjustment)->lower)
		positionY = ((GtkAdjustment *)screen.win.vAdjustment)->lower;
	if (positionY > (((GtkAdjustment *)screen.win.vAdjustment)->upper - ((GtkAdjustment *)screen.win.vAdjustment)->page_size))
		positionY = (((GtkAdjustment *)screen.win.vAdjustment)->upper - ((GtkAdjustment *)screen.win.vAdjustment)->page_size);
	gtk_adjustment_set_value ((GtkAdjustment *)screen.win.vAdjustment, positionY);
}