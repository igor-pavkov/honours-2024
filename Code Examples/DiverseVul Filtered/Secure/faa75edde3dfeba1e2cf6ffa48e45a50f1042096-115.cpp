static void lsr_get_color(GF_LASeRCodec *lsr, u32 idx, SVG_Color *color)
{
	LSRCol *c;
	if (idx>=lsr->nb_cols) return;

	c = &lsr->col_table[idx];
	color->red = INT2FIX(c->r) / lsr->color_scale;
	color->green = INT2FIX(c->g) / lsr->color_scale;
	color->blue = INT2FIX(c->b) / lsr->color_scale;
	color->type = SVG_COLOR_RGBCOLOR;
}