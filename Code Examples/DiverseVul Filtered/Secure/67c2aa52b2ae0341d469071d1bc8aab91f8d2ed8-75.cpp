static const char* altsec_order_string(BYTE orderType)
{
	const char* orders[] = {
		"[0x%02" PRIx8 "] Switch Surface",         "[0x%02" PRIx8 "] Create Offscreen Bitmap",
		"[0x%02" PRIx8 "] Stream Bitmap First",    "[0x%02" PRIx8 "] Stream Bitmap Next",
		"[0x%02" PRIx8 "] Create NineGrid Bitmap", "[0x%02" PRIx8 "] Draw GDI+ First",
		"[0x%02" PRIx8 "] Draw GDI+ Next",         "[0x%02" PRIx8 "] Draw GDI+ End",
		"[0x%02" PRIx8 "] Draw GDI+ Cache First",  "[0x%02" PRIx8 "] Draw GDI+ Cache Next",
		"[0x%02" PRIx8 "] Draw GDI+ Cache End",    "[0x%02" PRIx8 "] Windowing",
		"[0x%02" PRIx8 "] Desktop Composition",    "[0x%02" PRIx8 "] Frame Marker"
	};
	const char* fmt = "[0x%02" PRIx8 "] UNKNOWN";
	static char buffer[64] = { 0 };

	if (orderType < ARRAYSIZE(orders))
		fmt = orders[orderType];

	sprintf_s(buffer, ARRAYSIZE(buffer), fmt, orderType);
	return buffer;
}