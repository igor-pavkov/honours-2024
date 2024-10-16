bool is_deferred_open_async(const struct deferred_open_record *rec)
{
	return rec->async_open;
}