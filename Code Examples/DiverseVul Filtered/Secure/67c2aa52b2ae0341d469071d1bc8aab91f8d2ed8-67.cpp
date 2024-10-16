static const char* primary_order_string(UINT32 orderType)
{
	const char* orders[] = { "[0x%02" PRIx8 "] DstBlt",
		                     "[0x%02" PRIx8 "] PatBlt",
		                     "[0x%02" PRIx8 "] ScrBlt",
		                     "[0x%02" PRIx8 "] UNUSED",
		                     "[0x%02" PRIx8 "] UNUSED",
		                     "[0x%02" PRIx8 "] UNUSED",
		                     "[0x%02" PRIx8 "] UNUSED",
		                     "[0x%02" PRIx8 "] DrawNineGrid",
		                     "[0x%02" PRIx8 "] MultiDrawNineGrid",
		                     "[0x%02" PRIx8 "] LineTo",
		                     "[0x%02" PRIx8 "] OpaqueRect",
		                     "[0x%02" PRIx8 "] SaveBitmap",
		                     "[0x%02" PRIx8 "] UNUSED",
		                     "[0x%02" PRIx8 "] MemBlt",
		                     "[0x%02" PRIx8 "] Mem3Blt",
		                     "[0x%02" PRIx8 "] MultiDstBlt",
		                     "[0x%02" PRIx8 "] MultiPatBlt",
		                     "[0x%02" PRIx8 "] MultiScrBlt",
		                     "[0x%02" PRIx8 "] MultiOpaqueRect",
		                     "[0x%02" PRIx8 "] FastIndex",
		                     "[0x%02" PRIx8 "] PolygonSC",
		                     "[0x%02" PRIx8 "] PolygonCB",
		                     "[0x%02" PRIx8 "] Polyline",
		                     "[0x%02" PRIx8 "] UNUSED",
		                     "[0x%02" PRIx8 "] FastGlyph",
		                     "[0x%02" PRIx8 "] EllipseSC",
		                     "[0x%02" PRIx8 "] EllipseCB",
		                     "[0x%02" PRIx8 "] GlyphIndex" };
	const char* fmt = "[0x%02" PRIx8 "] UNKNOWN";
	static char buffer[64] = { 0 };

	if (orderType < ARRAYSIZE(orders))
		fmt = orders[orderType];

	sprintf_s(buffer, ARRAYSIZE(buffer), fmt, orderType);
	return buffer;
}