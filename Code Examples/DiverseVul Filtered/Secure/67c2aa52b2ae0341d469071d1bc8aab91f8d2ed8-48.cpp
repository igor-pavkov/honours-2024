static BOOL check_order_activated(wLog* log, rdpSettings* settings, const char* orderName,
                                  BOOL condition)
{
	if (!condition)
	{
		if (settings->AllowUnanouncedOrdersFromServer)
		{
			WLog_Print(log, WLOG_WARN,
			           "%s - SERVER BUG: The support for this feature was not announced!",
			           orderName);
			return TRUE;
		}
		else
		{
			WLog_Print(log, WLOG_ERROR,
			           "%s - SERVER BUG: The support for this feature was not announced! Use "
			           "/relax-order-checks to ignore",
			           orderName);
			return FALSE;
		}
	}

	return TRUE;
}