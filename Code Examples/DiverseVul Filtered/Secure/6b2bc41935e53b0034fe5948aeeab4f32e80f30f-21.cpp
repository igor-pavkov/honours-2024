static BOOL update_read_notify_icon_infotip(wStream* s, NOTIFY_ICON_INFOTIP* notifyIconInfoTip)
{
	if (Stream_GetRemainingLength(s) < 8)
		return FALSE;

	Stream_Read_UINT32(s, notifyIconInfoTip->timeout);              /* timeout (4 bytes) */
	Stream_Read_UINT32(s, notifyIconInfoTip->flags);                /* infoFlags (4 bytes) */
	return rail_read_unicode_string(s, &notifyIconInfoTip->text) && /* infoTipText */
	       rail_read_unicode_string(s, &notifyIconInfoTip->title);  /* title */
}