BOOL update_post_connect(rdpUpdate* update)
{
	update->asynchronous = update->context->settings->AsyncUpdate;

	if (update->asynchronous)
		if (!(update->proxy = update_message_proxy_new(update)))
			return FALSE;

	update->altsec->switch_surface.bitmapId = SCREEN_BITMAP_SURFACE;
	IFCALL(update->altsec->SwitchSurface, update->context, &(update->altsec->switch_surface));
	update->initialState = FALSE;
	return TRUE;
}