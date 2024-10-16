static UINT16 update_calculate_new_or_existing_window(const WINDOW_ORDER_INFO* orderInfo,
                                                      const WINDOW_STATE_ORDER* stateOrder)
{
	UINT16 orderSize = 11;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_OWNER) != 0)
		orderSize += 4;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_STYLE) != 0)
		orderSize += 8;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_SHOW) != 0)
		orderSize += 1;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TITLE) != 0)
		orderSize += 2 + stateOrder->titleInfo.length;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_OFFSET) != 0)
		orderSize += 8;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_SIZE) != 0)
		orderSize += 8;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RESIZE_MARGIN_X) != 0)
		orderSize += 8;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RESIZE_MARGIN_Y) != 0)
		orderSize += 8;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RP_CONTENT) != 0)
		orderSize += 1;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ROOT_PARENT) != 0)
		orderSize += 4;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET) != 0)
		orderSize += 8;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_CLIENT_DELTA) != 0)
		orderSize += 8;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE) != 0)
		orderSize += 8;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS) != 0)
		orderSize += 2 + stateOrder->numWindowRects * sizeof(RECTANGLE_16);

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET) != 0)
		orderSize += 8;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY) != 0)
		orderSize += 2 + stateOrder->numVisibilityRects * sizeof(RECTANGLE_16);

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_OVERLAY_DESCRIPTION) != 0)
		orderSize += 2 + stateOrder->OverlayDescription.length;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TASKBAR_BUTTON) != 0)
		orderSize += 1;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ENFORCE_SERVER_ZORDER) != 0)
		orderSize += 1;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_APPBAR_STATE) != 0)
		orderSize += 1;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_APPBAR_EDGE) != 0)
		orderSize += 1;

	return orderSize;
}