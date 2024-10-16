static BOOL update_read_window_state_order(wStream* s, WINDOW_ORDER_INFO* orderInfo,
                                           WINDOW_STATE_ORDER* windowState)
{
	UINT32 i;
	size_t size;
	RECTANGLE_16* newRect;

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_OWNER)
	{
		if (Stream_GetRemainingLength(s) < 4)
			return FALSE;

		Stream_Read_UINT32(s, windowState->ownerWindowId); /* ownerWindowId (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_STYLE)
	{
		if (Stream_GetRemainingLength(s) < 8)
			return FALSE;

		Stream_Read_UINT32(s, windowState->style);         /* style (4 bytes) */
		Stream_Read_UINT32(s, windowState->extendedStyle); /* extendedStyle (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_SHOW)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, windowState->showState); /* showState (1 byte) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TITLE)
	{
		if (!rail_read_unicode_string(s, &windowState->titleInfo)) /* titleInfo */
			return FALSE;
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_OFFSET)
	{
		if (Stream_GetRemainingLength(s) < 8)
			return FALSE;

		Stream_Read_INT32(s, windowState->clientOffsetX); /* clientOffsetX (4 bytes) */
		Stream_Read_INT32(s, windowState->clientOffsetY); /* clientOffsetY (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_SIZE)
	{
		if (Stream_GetRemainingLength(s) < 8)
			return FALSE;

		Stream_Read_UINT32(s, windowState->clientAreaWidth);  /* clientAreaWidth (4 bytes) */
		Stream_Read_UINT32(s, windowState->clientAreaHeight); /* clientAreaHeight (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RESIZE_MARGIN_X)
	{
		if (Stream_GetRemainingLength(s) < 8)
			return FALSE;

		Stream_Read_UINT32(s, windowState->resizeMarginLeft);
		Stream_Read_UINT32(s, windowState->resizeMarginRight);
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RESIZE_MARGIN_Y)
	{
		if (Stream_GetRemainingLength(s) < 8)
			return FALSE;

		Stream_Read_UINT32(s, windowState->resizeMarginTop);
		Stream_Read_UINT32(s, windowState->resizeMarginBottom);
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RP_CONTENT)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, windowState->RPContent); /* RPContent (1 byte) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ROOT_PARENT)
	{
		if (Stream_GetRemainingLength(s) < 4)
			return FALSE;

		Stream_Read_UINT32(s, windowState->rootParentHandle); /* rootParentHandle (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET)
	{
		if (Stream_GetRemainingLength(s) < 8)
			return FALSE;

		Stream_Read_INT32(s, windowState->windowOffsetX); /* windowOffsetX (4 bytes) */
		Stream_Read_INT32(s, windowState->windowOffsetY); /* windowOffsetY (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_CLIENT_DELTA)
	{
		if (Stream_GetRemainingLength(s) < 8)
			return FALSE;

		Stream_Read_INT32(s, windowState->windowClientDeltaX); /* windowClientDeltaX (4 bytes) */
		Stream_Read_INT32(s, windowState->windowClientDeltaY); /* windowClientDeltaY (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE)
	{
		if (Stream_GetRemainingLength(s) < 8)
			return FALSE;

		Stream_Read_UINT32(s, windowState->windowWidth);  /* windowWidth (4 bytes) */
		Stream_Read_UINT32(s, windowState->windowHeight); /* windowHeight (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS)
	{
		if (Stream_GetRemainingLength(s) < 2)
			return FALSE;

		Stream_Read_UINT16(s, windowState->numWindowRects); /* numWindowRects (2 bytes) */

		if (windowState->numWindowRects == 0)
		{
			return TRUE;
		}

		size = sizeof(RECTANGLE_16) * windowState->numWindowRects;
		newRect = (RECTANGLE_16*)realloc(windowState->windowRects, size);

		if (!newRect)
		{
			free(windowState->windowRects);
			windowState->windowRects = NULL;
			return FALSE;
		}

		windowState->windowRects = newRect;

		if (Stream_GetRemainingLength(s) < 8 * windowState->numWindowRects)
			return FALSE;

		/* windowRects */
		for (i = 0; i < windowState->numWindowRects; i++)
		{
			Stream_Read_UINT16(s, windowState->windowRects[i].left);   /* left (2 bytes) */
			Stream_Read_UINT16(s, windowState->windowRects[i].top);    /* top (2 bytes) */
			Stream_Read_UINT16(s, windowState->windowRects[i].right);  /* right (2 bytes) */
			Stream_Read_UINT16(s, windowState->windowRects[i].bottom); /* bottom (2 bytes) */
		}
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET)
	{
		if (Stream_GetRemainingLength(s) < 8)
			return FALSE;

		Stream_Read_UINT32(s, windowState->visibleOffsetX); /* visibleOffsetX (4 bytes) */
		Stream_Read_UINT32(s, windowState->visibleOffsetY); /* visibleOffsetY (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY)
	{
		if (Stream_GetRemainingLength(s) < 2)
			return FALSE;

		Stream_Read_UINT16(s, windowState->numVisibilityRects); /* numVisibilityRects (2 bytes) */

		if (windowState->numVisibilityRects != 0)
		{
			size = sizeof(RECTANGLE_16) * windowState->numVisibilityRects;
			newRect = (RECTANGLE_16*)realloc(windowState->visibilityRects, size);

			if (!newRect)
			{
				free(windowState->visibilityRects);
				windowState->visibilityRects = NULL;
				return FALSE;
			}

			windowState->visibilityRects = newRect;

			if (Stream_GetRemainingLength(s) < windowState->numVisibilityRects * 8)
				return FALSE;

			/* visibilityRects */
			for (i = 0; i < windowState->numVisibilityRects; i++)
			{
				Stream_Read_UINT16(s, windowState->visibilityRects[i].left);  /* left (2 bytes) */
				Stream_Read_UINT16(s, windowState->visibilityRects[i].top);   /* top (2 bytes) */
				Stream_Read_UINT16(s, windowState->visibilityRects[i].right); /* right (2 bytes) */
				Stream_Read_UINT16(s,
				                   windowState->visibilityRects[i].bottom); /* bottom (2 bytes) */
			}
		}
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_OVERLAY_DESCRIPTION)
	{
		if (!rail_read_unicode_string(s, &windowState->OverlayDescription))
			return FALSE;
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ICON_OVERLAY_NULL)
	{
		/* no data to be read here */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TASKBAR_BUTTON)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, windowState->TaskbarButton);
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ENFORCE_SERVER_ZORDER)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, windowState->EnforceServerZOrder);
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_APPBAR_STATE)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, windowState->AppBarState);
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_APPBAR_EDGE)
	{
		if (Stream_GetRemainingLength(s) < 1)
			return FALSE;

		Stream_Read_UINT8(s, windowState->AppBarEdge);
	}

	return TRUE;
}