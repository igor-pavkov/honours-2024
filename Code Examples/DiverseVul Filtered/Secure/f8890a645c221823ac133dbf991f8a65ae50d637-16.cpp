BOOL update_begin_paint(rdpUpdate* update)
{
	if (!update)
		return FALSE;

	EnterCriticalSection(&update->mux);

	if (!update->BeginPaint)
		return TRUE;

	return update->BeginPaint(update->context);
}