static bool is_valid_type(enum dm_queue_mode cur, enum dm_queue_mode new)
{
	if (cur == new ||
	    (cur == DM_TYPE_BIO_BASED && new == DM_TYPE_DAX_BIO_BASED))
		return true;

	return false;
}