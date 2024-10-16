BOOL update_recv_order(rdpUpdate* update, wStream* s)
{
	BOOL rc;
	BYTE controlFlags;

	if (Stream_GetRemainingLength(s) < 1)
	{
		WLog_Print(update->log, WLOG_ERROR, "Stream_GetRemainingLength(s) < 1");
		return FALSE;
	}

	Stream_Read_UINT8(s, controlFlags); /* controlFlags (1 byte) */

	if (!(controlFlags & ORDER_STANDARD))
		rc = update_recv_altsec_order(update, s, controlFlags);
	else if (controlFlags & ORDER_SECONDARY)
		rc = update_recv_secondary_order(update, s, controlFlags);
	else
		rc = update_recv_primary_order(update, s, controlFlags);

	if (!rc)
		WLog_Print(update->log, WLOG_ERROR, "order flags %02" PRIx8 " failed", controlFlags);

	return rc;
}