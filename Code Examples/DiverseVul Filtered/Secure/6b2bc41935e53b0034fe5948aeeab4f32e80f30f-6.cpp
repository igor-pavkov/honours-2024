static BOOL update_recv_notification_icon_info_order(rdpUpdate* update, wStream* s,
                                                     WINDOW_ORDER_INFO* orderInfo)
{
	rdpContext* context = update->context;
	rdpWindowUpdate* window = update->window;
	BOOL result = TRUE;

	if (Stream_GetRemainingLength(s) < 8)
		return FALSE;

	Stream_Read_UINT32(s, orderInfo->windowId);     /* windowId (4 bytes) */
	Stream_Read_UINT32(s, orderInfo->notifyIconId); /* notifyIconId (4 bytes) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_DELETED)
	{
		update_read_notification_icon_delete_order(s, orderInfo);
		WLog_Print(update->log, WLOG_DEBUG, "NotifyIconDelete");
		IFCALLRET(window->NotifyIconDelete, result, context, orderInfo);
	}
	else
	{
		NOTIFY_ICON_STATE_ORDER notify_icon_state = { 0 };
		result = update_read_notification_icon_state_order(s, orderInfo, &notify_icon_state);

		if (!result)
			goto fail;

		if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_NEW)
		{
			WLog_Print(update->log, WLOG_DEBUG, "NotifyIconCreate");
			IFCALLRET(window->NotifyIconCreate, result, context, orderInfo, &notify_icon_state);
		}
		else
		{
			WLog_Print(update->log, WLOG_DEBUG, "NotifyIconUpdate");
			IFCALLRET(window->NotifyIconUpdate, result, context, orderInfo, &notify_icon_state);
		}
	fail:
		update_notify_icon_state_order_free(&notify_icon_state);
	}

	return result;
}