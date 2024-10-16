void callbacks_update_scrollbar_limits (void){
	gerbv_render_info_t tempRenderInfo = {0, 0, 0, 0,
		GERBV_RENDER_TYPE_CAIRO_HIGH_QUALITY,
		screenRenderInfo.displayWidth,
		screenRenderInfo.displayHeight};

	GtkAdjustment *hAdjust = (GtkAdjustment *)screen.win.hAdjustment;
	GtkAdjustment *vAdjust = (GtkAdjustment *)screen.win.vAdjustment;
	gerbv_render_zoom_to_fit_display (mainProject, &tempRenderInfo);
	hAdjust->lower = tempRenderInfo.lowerLeftX;
	hAdjust->page_increment = hAdjust->page_size;
	hAdjust->step_increment = hAdjust->page_size / 10.0;
	vAdjust->lower = tempRenderInfo.lowerLeftY;
	vAdjust->page_increment = vAdjust->page_size;
	vAdjust->step_increment = vAdjust->page_size / 10.0;
	hAdjust->upper = tempRenderInfo.lowerLeftX + (tempRenderInfo.displayWidth / tempRenderInfo.scaleFactorX);
	hAdjust->page_size = screenRenderInfo.displayWidth / screenRenderInfo.scaleFactorX;
	vAdjust->upper = tempRenderInfo.lowerLeftY + (tempRenderInfo.displayHeight / tempRenderInfo.scaleFactorY);
	vAdjust->page_size = screenRenderInfo.displayHeight / screenRenderInfo.scaleFactorY;
	callbacks_update_scrollbar_positions ();
}