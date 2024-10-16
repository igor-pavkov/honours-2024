static void _php_image_bw_convert(gdImagePtr im_org, gdIOCtx *out, int threshold)
{
	gdImagePtr im_dest;
	int white, black;
	int color, color_org, median;
	int dest_height = gdImageSY(im_org);
	int dest_width = gdImageSX(im_org);
	int x, y;

	im_dest = gdImageCreate(dest_width, dest_height);
	if (im_dest == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to allocate temporary buffer");
		return;
	}

	white = gdImageColorAllocate(im_dest, 255, 255, 255);
	if (white == -1) {
		php_error_docref(NULL, E_WARNING, "Unable to allocate the colors for the destination buffer");
		return;
	}

	black = gdImageColorAllocate(im_dest, 0, 0, 0);
	if (black == -1) {
		php_error_docref(NULL, E_WARNING, "Unable to allocate the colors for the destination buffer");
		return;
	}

	if (im_org->trueColor) {
		gdImageTrueColorToPalette(im_org, 1, 256);
	}

	for (y = 0; y < dest_height; y++) {
		for (x = 0; x < dest_width; x++) {
			color_org = gdImageGetPixel(im_org, x, y);
			median = (im_org->red[color_org] + im_org->green[color_org] + im_org->blue[color_org]) / 3;
			if (median < threshold) {
				color = black;
			} else {
				color = white;
			}
			gdImageSetPixel (im_dest, x, y, color);
		}
	}
	gdImageWBMPCtx (im_dest, black, out);

}