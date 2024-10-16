static BOOL window_order_supported(const rdpSettings* settings, UINT32 fieldFlags)
{
	const UINT32 mask = (WINDOW_ORDER_FIELD_CLIENT_AREA_SIZE | WINDOW_ORDER_FIELD_RP_CONTENT |
	                     WINDOW_ORDER_FIELD_ROOT_PARENT);
	BOOL dresult;

	if (!settings)
		return FALSE;

	/* See [MS-RDPERP] 2.2.1.1.2 Window List Capability Set */
	dresult = settings->AllowUnanouncedOrdersFromServer;

	switch (settings->RemoteWndSupportLevel)
	{
		case WINDOW_LEVEL_SUPPORTED_EX:
			return TRUE;

		case WINDOW_LEVEL_SUPPORTED:
			return ((fieldFlags & mask) == 0) || dresult;

		case WINDOW_LEVEL_NOT_SUPPORTED:
			return dresult;

		default:
			return dresult;
	}
}