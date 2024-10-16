static const char* secondary_order_string(UINT32 orderType)
{
	const char* orders[] = { "[0x%02" PRIx8 "] Cache Bitmap",
		                     "[0x%02" PRIx8 "] Cache Color Table",
		                     "[0x%02" PRIx8 "] Cache Bitmap (Compressed)",
		                     "[0x%02" PRIx8 "] Cache Glyph",
		                     "[0x%02" PRIx8 "] Cache Bitmap V2",
		                     "[0x%02" PRIx8 "] Cache Bitmap V2 (Compressed)",
		                     "[0x%02" PRIx8 "] UNUSED",
		                     "[0x%02" PRIx8 "] Cache Brush",
		                     "[0x%02" PRIx8 "] Cache Bitmap V3" };
	const char* fmt = "[0x%02" PRIx8 "] UNKNOWN";
	static char buffer[64] = { 0 };

	if (orderType < ARRAYSIZE(orders))
		fmt = orders[orderType];

	sprintf_s(buffer, ARRAYSIZE(buffer), fmt, orderType);
	return buffer;
}