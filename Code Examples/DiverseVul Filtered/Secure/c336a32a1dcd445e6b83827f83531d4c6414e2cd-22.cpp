static void SelectRendition()
{
	int j, i = 0;
	int attr = curr->w_rend.attr;
	int colorbg = curr->w_rend.colorbg;
	int colorfg = curr->w_rend.colorfg;

	do {
		j = curr->w_args[i];
		/* indexed colour space aka 256 colours; example escape \e[48;2;12m */
		if ((j == 38 || j == 48) && i + 2 < curr->w_NumArgs && curr->w_args[i + 1] == 5) {
			int jj;

			i += 2;
			jj = curr->w_args[i];
			if (jj < 0 || jj > 255)
				continue;
			if (j == 38) {
				colorfg = jj | 0x01000000;
			} else {
				colorbg = jj | 0x01000000;
			}
			continue;
		}
		/* truecolor (24bit) colour space; example escape \e[48;5;12;13;14m 
		 * where 12;13;14 are rgb values */
		if ((j == 38 || j == 48) && i + 4 < curr->w_NumArgs && curr->w_args[i + 1] == 2) {
			uint8_t r, g, b;

			r = curr->w_args[i + 2];
			g = curr->w_args[i + 3];
			b = curr->w_args[i + 4];

			if (j == 38) {
				colorfg = 0x02000000 | (r << 16) | (g << 8) | b;
			} else {
				colorbg = 0x02000000 | (r << 16) | (g << 8) | b;
			}
			i += 4;
			continue;
		}
		if (j >= 90 && j <= 97)
			colorfg = (j - 90 + 8) | 0x01000000;
		if (j >= 100 && j <= 107)
			colorbg = (j - 100 + 8) | 0x01000000;
		if (j >= 30 && j < 38)
			colorfg = (j - 30) | 0x01000000;
		if (j >= 40 && j < 48)
			colorbg = (j - 40) | 0x01000000;
		if (j == 39)
			colorfg = 0;
		if (j == 49)
			colorbg = 0;
		if (j == 0) {
			attr = 0;
			/* will be xored to 0 */
			colorbg = 0;
			colorfg = 0;
		}

		if (j < 0 || j >= (int)(sizeof(rendlist)/sizeof(*rendlist)))
			continue;
		j = rendlist[j];
		if (j & (1 << NATTR))
			attr &= j;
		else
			attr |= j;
	} while (++i < curr->w_NumArgs);
	
	curr->w_rend.attr = attr;
	
	curr->w_rend.colorbg = colorbg;
	curr->w_rend.colorfg = colorfg;
	LSetRendition(&curr->w_layer, &curr->w_rend);
}