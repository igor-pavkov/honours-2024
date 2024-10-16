void gdImageSetPixel (gdImagePtr im, int x, int y, int color)
{
	int p;
	switch (color) {
		case gdStyled:
			if (!im->style) {
				/* Refuse to draw if no style is set. */
				return;
			} else {
				p = im->style[im->stylePos++];
			}
			if (p != gdTransparent) {
				gdImageSetPixel(im, x, y, p);
			}
			im->stylePos = im->stylePos % im->styleLength;
			break;
		case gdStyledBrushed:
			if (!im->style) {
				/* Refuse to draw if no style is set. */
				return;
			}
			p = im->style[im->stylePos++];
			if (p != gdTransparent && p != 0) {
				gdImageSetPixel(im, x, y, gdBrushed);
			}
			im->stylePos = im->stylePos % im->styleLength;
			break;
		case gdBrushed:
			gdImageBrushApply(im, x, y);
			break;
		case gdTiled:
			gdImageTileApply(im, x, y);
			break;
		case gdAntiAliased:
			gdImageAntiAliasedApply(im, x, y);
			break;
		default:
			if (gdImageBoundsSafe(im, x, y)) {
				if (im->trueColor) {
					switch (im->alphaBlendingFlag) {
						default:
						case gdEffectReplace:
							im->tpixels[y][x] = color;
							break;
						case gdEffectAlphaBlend:
							im->tpixels[y][x] = gdAlphaBlend(im->tpixels[y][x], color);
							break;
						case gdEffectNormal:
							im->tpixels[y][x] = gdAlphaBlend(im->tpixels[y][x], color);
							break;
						case gdEffectOverlay :
							im->tpixels[y][x] = gdLayerOverlay(im->tpixels[y][x], color);
							break;
					}
				} else {
					im->pixels[y][x] = color;
				}
			}
			break;
	}
}