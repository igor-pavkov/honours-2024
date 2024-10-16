void gdImageAABlend (gdImagePtr im)
{
	float p_alpha, old_alpha;
	int color = im->AA_color, color_red, color_green, color_blue;
	int old_color, old_red, old_green, old_blue;
	int p_color, p_red, p_green, p_blue;
	int px, py;

	color_red = gdImageRed(im, color);
	color_green = gdImageGreen(im, color);
	color_blue = gdImageBlue(im, color);

	/* Impose the anti-aliased drawing on the image. */
	for (py = 0; py < im->sy; py++) {
		for (px = 0; px < im->sx; px++) {
			if (im->AA_opacity[py][px] != 0) {
				old_color = gdImageGetPixel(im, px, py);

				if ((old_color != color) && ((old_color != im->AA_dont_blend) || (im->AA_opacity[py][px] == 255))) {
					/* Only blend with different colors that aren't the dont_blend color. */
					p_alpha = (float) (im->AA_opacity[py][px]) / 255.0;
					old_alpha = 1.0 - p_alpha;

					if (p_alpha >= 1.0) {
						p_color = color;
					} else {
						old_red = gdImageRed(im, old_color);
						old_green = gdImageGreen(im, old_color);
						old_blue = gdImageBlue(im, old_color);

						p_red = (int) (((float) color_red * p_alpha) + ((float) old_red * old_alpha));
						p_green = (int) (((float) color_green * p_alpha) + ((float) old_green * old_alpha));
						p_blue = (int) (((float) color_blue * p_alpha) + ((float) old_blue * old_alpha));
						p_color = gdImageColorResolve(im, p_red, p_green, p_blue);
					}
					gdImageSetPixel(im, px, py, p_color);
				}
			}
		}
		/* Clear the AA_opacity array behind us. */
		memset(im->AA_opacity[py], 0, im->sx);
	}
}