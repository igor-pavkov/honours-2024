static void update_notify_icon_state_order_free(NOTIFY_ICON_STATE_ORDER* notify)
{
	free(notify->toolTip.string);
	free(notify->infoTip.text.string);
	free(notify->infoTip.title.string);
	update_free_window_icon_info(&notify->icon);
	memset(notify, 0, sizeof(NOTIFY_ICON_STATE_ORDER));
}