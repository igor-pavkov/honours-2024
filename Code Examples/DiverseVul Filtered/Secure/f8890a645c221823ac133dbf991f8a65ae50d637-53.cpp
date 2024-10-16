static BOOL update_send_notify_icon_create(rdpContext* context, const WINDOW_ORDER_INFO* orderInfo,
                                           const NOTIFY_ICON_STATE_ORDER* iconStateOrder)
{
	return update_send_new_or_existing_notification_icons(context, orderInfo, iconStateOrder);
}