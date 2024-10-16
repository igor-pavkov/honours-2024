static BOOL update_check_flush(rdpContext* context, int size)
{
	wStream* s;
	rdpUpdate* update = context->update;
	s = update->us;

	if (!update->us)
	{
		update_begin_paint(update);
		return FALSE;
	}

	if (Stream_GetPosition(s) + size + 64 >= 0x3FFF)
	{
		update_flush(context);
		return TRUE;
	}

	return FALSE;
}