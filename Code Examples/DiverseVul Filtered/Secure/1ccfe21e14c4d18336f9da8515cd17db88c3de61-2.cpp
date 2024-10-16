BGD_DECLARE(gdImagePtr) gdImageCrop(gdImagePtr src, const gdRect *crop)
{
	gdImagePtr dst;

	dst = gdImageCreateTrueColor(crop->width, crop->height);
	if (!dst) return NULL;
	gdImageCopy(dst, src, 0, 0, crop->x, crop->y, crop->width, crop->height);

	return dst;
}