static BOOL update_send_new_or_existing_window(rdpContext* context,
                                               const WINDOW_ORDER_INFO* orderInfo,
                                               const WINDOW_STATE_ORDER* stateOrder)
{
	wStream* s;
	rdpUpdate* update = context->update;
	BYTE controlFlags = ORDER_SECONDARY | (ORDER_TYPE_WINDOW << 2);
	UINT16 orderSize = update_calculate_new_or_existing_window(orderInfo, stateOrder);

	update_check_flush(context, orderSize);

	s = update->us;

	if (!s)
		return FALSE;

	if (!Stream_EnsureRemainingCapacity(s, orderSize))
		return FALSE;

	Stream_Write_UINT8(s, controlFlags);           /* Header (1 byte) */
	Stream_Write_UINT16(s, orderSize);             /* OrderSize (2 bytes) */
	Stream_Write_UINT32(s, orderInfo->fieldFlags); /* FieldsPresentFlags (4 bytes) */
	Stream_Write_UINT32(s, orderInfo->windowId);   /* WindowID (4 bytes) */

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_OWNER) != 0)
		Stream_Write_UINT32(s, stateOrder->ownerWindowId);

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_STYLE) != 0)
	{
		Stream_Write_UINT32(s, stateOrder->style);
		Stream_Write_UINT32(s, stateOrder->extendedStyle);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_SHOW) != 0)
	{
		Stream_Write_UINT8(s, stateOrder->showState);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TITLE) != 0)
	{
		Stream_Write_UINT16(s, stateOrder->titleInfo.length);
		Stream_Write(s, stateOrder->titleInfo.string, stateOrder->titleInfo.length);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_OFFSET) != 0)
	{
		Stream_Write_INT32(s, stateOrder->clientOffsetX);
		Stream_Write_INT32(s, stateOrder->clientOffsetY);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_SIZE) != 0)
	{
		Stream_Write_UINT32(s, stateOrder->clientAreaWidth);
		Stream_Write_UINT32(s, stateOrder->clientAreaHeight);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RESIZE_MARGIN_X) != 0)
	{
		Stream_Write_UINT32(s, stateOrder->resizeMarginLeft);
		Stream_Write_UINT32(s, stateOrder->resizeMarginRight);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RESIZE_MARGIN_Y) != 0)
	{
		Stream_Write_UINT32(s, stateOrder->resizeMarginTop);
		Stream_Write_UINT32(s, stateOrder->resizeMarginBottom);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RP_CONTENT) != 0)
	{
		Stream_Write_UINT8(s, stateOrder->RPContent);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ROOT_PARENT) != 0)
	{
		Stream_Write_UINT32(s, stateOrder->rootParentHandle);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET) != 0)
	{
		Stream_Write_INT32(s, stateOrder->windowOffsetX);
		Stream_Write_INT32(s, stateOrder->windowOffsetY);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_CLIENT_DELTA) != 0)
	{
		Stream_Write_INT32(s, stateOrder->windowClientDeltaX);
		Stream_Write_INT32(s, stateOrder->windowClientDeltaY);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE) != 0)
	{
		Stream_Write_UINT32(s, stateOrder->windowWidth);
		Stream_Write_UINT32(s, stateOrder->windowHeight);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS) != 0)
	{
		Stream_Write_UINT16(s, stateOrder->numWindowRects);
		Stream_Write(s, stateOrder->windowRects, stateOrder->numWindowRects * sizeof(RECTANGLE_16));
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET) != 0)
	{
		Stream_Write_UINT32(s, stateOrder->visibleOffsetX);
		Stream_Write_UINT32(s, stateOrder->visibleOffsetY);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY) != 0)
	{
		Stream_Write_UINT16(s, stateOrder->numVisibilityRects);
		Stream_Write(s, stateOrder->visibilityRects,
		             stateOrder->numVisibilityRects * sizeof(RECTANGLE_16));
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_OVERLAY_DESCRIPTION) != 0)
	{
		Stream_Write_UINT16(s, stateOrder->OverlayDescription.length);
		Stream_Write(s, stateOrder->OverlayDescription.string,
		             stateOrder->OverlayDescription.length);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TASKBAR_BUTTON) != 0)
	{
		Stream_Write_UINT8(s, stateOrder->TaskbarButton);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ENFORCE_SERVER_ZORDER) != 0)
	{
		Stream_Write_UINT8(s, stateOrder->EnforceServerZOrder);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_APPBAR_STATE) != 0)
	{
		Stream_Write_UINT8(s, stateOrder->AppBarState);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_APPBAR_EDGE) != 0)
	{
		Stream_Write_UINT8(s, stateOrder->AppBarEdge);
	}

	update->numberOrders++;
	return TRUE;
}