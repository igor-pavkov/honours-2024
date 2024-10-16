static BOOL update_recv_orders(rdpUpdate* update, wStream* s)
{
	UINT16 numberOrders;

	if (Stream_GetRemainingLength(s) < 6)
	{
		WLog_ERR(TAG, "Stream_GetRemainingLength(s) < 6");
		return FALSE;
	}

	Stream_Seek_UINT16(s);               /* pad2OctetsA (2 bytes) */
	Stream_Read_UINT16(s, numberOrders); /* numberOrders (2 bytes) */
	Stream_Seek_UINT16(s);               /* pad2OctetsB (2 bytes) */

	while (numberOrders > 0)
	{
		if (!update_recv_order(update, s))
		{
			WLog_ERR(TAG, "update_recv_order() failed");
			return FALSE;
		}

		numberOrders--;
	}

	return TRUE;
}