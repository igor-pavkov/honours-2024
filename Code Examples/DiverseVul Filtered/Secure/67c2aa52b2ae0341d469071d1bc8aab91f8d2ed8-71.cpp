static BOOL update_decompress_brush(wStream* s, BYTE* output, BYTE bpp)
{
	int index;
	int x, y, k;
	BYTE byte = 0;
	BYTE* palette;
	int bytesPerPixel;
	palette = Stream_Pointer(s) + 16;
	bytesPerPixel = ((bpp + 1) / 8);

	if (Stream_GetRemainingLength(s) < 16) // 64 / 4
		return FALSE;

	for (y = 7; y >= 0; y--)
	{
		for (x = 0; x < 8; x++)
		{
			if ((x % 4) == 0)
				Stream_Read_UINT8(s, byte);

			index = ((byte >> ((3 - (x % 4)) * 2)) & 0x03);

			for (k = 0; k < bytesPerPixel; k++)
			{
				output[((y * 8 + x) * bytesPerPixel) + k] = palette[(index * bytesPerPixel) + k];
			}
		}
	}

	return TRUE;
}