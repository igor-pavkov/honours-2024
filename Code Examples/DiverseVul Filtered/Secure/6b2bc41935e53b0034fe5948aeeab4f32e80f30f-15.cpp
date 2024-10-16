static void dump_window_state_order(wLog* log, const char* msg, const WINDOW_ORDER_INFO* order,
                                    const WINDOW_STATE_ORDER* state)
{
	char buffer[3000] = { 0 };
	const size_t bufferSize = sizeof(buffer) - 1;

	_snprintf(buffer, bufferSize, "%s windowId=0x%" PRIu32 "", msg, order->windowId);

	if (order->fieldFlags & WINDOW_ORDER_FIELD_OWNER)
		DUMP_APPEND(buffer, bufferSize, " owner=0x%" PRIx32 "", state->ownerWindowId);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_STYLE)
	{
		DUMP_APPEND(buffer, bufferSize, " [ex]style=<0x%" PRIx32 ", 0x%" PRIx32 "", state->style,
		            state->extendedStyle);
		if (state->style & WS_POPUP)
			DUMP_APPEND(buffer, bufferSize, " popup");
		if (state->style & WS_VISIBLE)
			DUMP_APPEND(buffer, bufferSize, " visible");
		if (state->style & WS_THICKFRAME)
			DUMP_APPEND(buffer, bufferSize, " thickframe");
		if (state->style & WS_BORDER)
			DUMP_APPEND(buffer, bufferSize, " border");
		if (state->style & WS_CAPTION)
			DUMP_APPEND(buffer, bufferSize, " caption");

		if (state->extendedStyle & WS_EX_NOACTIVATE)
			DUMP_APPEND(buffer, bufferSize, " noactivate");
		if (state->extendedStyle & WS_EX_TOOLWINDOW)
			DUMP_APPEND(buffer, bufferSize, " toolWindow");
		if (state->extendedStyle & WS_EX_TOPMOST)
			DUMP_APPEND(buffer, bufferSize, " topMost");

		DUMP_APPEND(buffer, bufferSize, ">");
	}

	if (order->fieldFlags & WINDOW_ORDER_FIELD_SHOW)
	{
		const char* showStr;
		switch (state->showState)
		{
			case 0:
				showStr = "hidden";
				break;
			case 2:
				showStr = "minimized";
				break;
			case 3:
				showStr = "maximized";
				break;
			case 5:
				showStr = "current";
				break;
			default:
				showStr = "<unknown>";
				break;
		}
		DUMP_APPEND(buffer, bufferSize, " show=%s", showStr);
	}

	if (order->fieldFlags & WINDOW_ORDER_FIELD_TITLE)
		DUMP_APPEND(buffer, bufferSize, " title");
	if (order->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_OFFSET)
		DUMP_APPEND(buffer, bufferSize, " clientOffset=(%" PRId32 ",%" PRId32 ")",
		            state->clientOffsetX, state->clientOffsetY);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_SIZE)
		DUMP_APPEND(buffer, bufferSize, " clientAreaWidth=%" PRIu32 " clientAreaHeight=%" PRIu32 "",
		            state->clientAreaWidth, state->clientAreaHeight);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_RESIZE_MARGIN_X)
		DUMP_APPEND(buffer, bufferSize,
		            " resizeMarginLeft=%" PRIu32 " resizeMarginRight=%" PRIu32 "",
		            state->resizeMarginLeft, state->resizeMarginRight);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_RESIZE_MARGIN_Y)
		DUMP_APPEND(buffer, bufferSize,
		            " resizeMarginTop=%" PRIu32 " resizeMarginBottom=%" PRIu32 "",
		            state->resizeMarginTop, state->resizeMarginBottom);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_RP_CONTENT)
		DUMP_APPEND(buffer, bufferSize, " rpContent=0x%" PRIx32 "", state->RPContent);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_ROOT_PARENT)
		DUMP_APPEND(buffer, bufferSize, " rootParent=0x%" PRIx32 "", state->rootParentHandle);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET)
		DUMP_APPEND(buffer, bufferSize, " windowOffset=(%" PRId32 ",%" PRId32 ")",
		            state->windowOffsetX, state->windowOffsetY);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_WND_CLIENT_DELTA)
		DUMP_APPEND(buffer, bufferSize, " windowClientDelta=(%" PRId32 ",%" PRId32 ")",
		            state->windowClientDeltaX, state->windowClientDeltaY);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE)
		DUMP_APPEND(buffer, bufferSize, " windowWidth=%" PRIu32 " windowHeight=%" PRIu32 "",
		            state->windowWidth, state->windowHeight);

	if (order->fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS)
	{
		UINT32 i;
		DUMP_APPEND(buffer, bufferSize, " windowRects=(");
		for (i = 0; i < state->numWindowRects; i++)
		{
			DUMP_APPEND(buffer, bufferSize, "(%" PRIu16 ",%" PRIu16 ",%" PRIu16 ",%" PRIu16 ")",
			            state->windowRects[i].left, state->windowRects[i].top,
			            state->windowRects[i].right, state->windowRects[i].bottom);
		}
		DUMP_APPEND(buffer, bufferSize, ")");
	}

	if (order->fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET)
		DUMP_APPEND(buffer, bufferSize, " visibleOffset=(%" PRId32 ",%" PRId32 ")",
		            state->visibleOffsetX, state->visibleOffsetY);

	if (order->fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY)
	{
		UINT32 i;
		DUMP_APPEND(buffer, bufferSize, " visibilityRects=(");
		for (i = 0; i < state->numVisibilityRects; i++)
		{
			DUMP_APPEND(buffer, bufferSize, "(%" PRIu16 ",%" PRIu16 ",%" PRIu16 ",%" PRIu16 ")",
			            state->visibilityRects[i].left, state->visibilityRects[i].top,
			            state->visibilityRects[i].right, state->visibilityRects[i].bottom);
		}
		DUMP_APPEND(buffer, bufferSize, ")");
	}

	if (order->fieldFlags & WINDOW_ORDER_FIELD_OVERLAY_DESCRIPTION)
		DUMP_APPEND(buffer, bufferSize, " overlayDescr");

	if (order->fieldFlags & WINDOW_ORDER_FIELD_ICON_OVERLAY_NULL)
		DUMP_APPEND(buffer, bufferSize, " iconOverlayNull");

	if (order->fieldFlags & WINDOW_ORDER_FIELD_TASKBAR_BUTTON)
		DUMP_APPEND(buffer, bufferSize, " taskBarButton=0x%" PRIx8 "", state->TaskbarButton);

	if (order->fieldFlags & WINDOW_ORDER_FIELD_ENFORCE_SERVER_ZORDER)
		DUMP_APPEND(buffer, bufferSize, " enforceServerZOrder=0x%" PRIx8 "",
		            state->EnforceServerZOrder);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_APPBAR_STATE)
		DUMP_APPEND(buffer, bufferSize, " appBarState=0x%" PRIx8 "", state->AppBarState);
	if (order->fieldFlags & WINDOW_ORDER_FIELD_APPBAR_EDGE)
	{
		const char* appBarEdgeStr;
		switch (state->AppBarEdge)
		{
			case 0:
				appBarEdgeStr = "left";
				break;
			case 1:
				appBarEdgeStr = "top";
				break;
			case 2:
				appBarEdgeStr = "right";
				break;
			case 3:
				appBarEdgeStr = "bottom";
				break;
			default:
				appBarEdgeStr = "<unknown>";
				break;
		}
		DUMP_APPEND(buffer, bufferSize, " appBarEdge=%s", appBarEdgeStr);
	}

	WLog_Print(log, WLOG_DEBUG, buffer);
}