static BOOL update_send_window_create(rdpContext* context, const WINDOW_ORDER_INFO* orderInfo,
                                      const WINDOW_STATE_ORDER* stateOrder)
{
	return update_send_new_or_existing_window(context, orderInfo, stateOrder);
}