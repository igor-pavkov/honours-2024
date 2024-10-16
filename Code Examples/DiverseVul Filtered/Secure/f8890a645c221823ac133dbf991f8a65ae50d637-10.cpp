static void update_flush(rdpContext* context)
{
	rdpUpdate* update = context->update;

	if (update->numberOrders > 0)
	{
		update_end_paint(update);
		update_begin_paint(update);
	}
}