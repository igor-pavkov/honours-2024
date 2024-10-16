void callbacks_update_ruler_pointers (void) {
	double xPosition, yPosition;
	xPosition = screenRenderInfo.lowerLeftX + (screen.last_x / screenRenderInfo.scaleFactorX);
	yPosition = screenRenderInfo.lowerLeftY + ((screenRenderInfo.displayHeight - screen.last_y) / screenRenderInfo.scaleFactorY);

	if (!((screen.unit == GERBV_MILS) && ((screenRenderInfo.scaleFactorX < 80)||(screenRenderInfo.scaleFactorY < 80)))) {
		xPosition = callbacks_calculate_actual_distance (xPosition);
		yPosition = callbacks_calculate_actual_distance (yPosition);
	}
	g_object_set (G_OBJECT (screen.win.hRuler), "position", xPosition, NULL);
	g_object_set (G_OBJECT (screen.win.vRuler), "position", yPosition, NULL);
}