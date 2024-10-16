static void _gdImageFillTiled(gdImagePtr im, int x, int y, int nc)
{
	int i, l, x1, x2, dy;
	int oc;   /* old pixel value */
	int wx2,wy2;
	/* stack of filled segments */
	struct seg *stack;
	struct seg *sp;
	char **pts;

	if (!im->tile) {
		return;
	}

	wx2=im->sx;wy2=im->sy;

	nc =  gdImageTileGet(im,x,y);

	pts = (char **) ecalloc(im->sy + 1, sizeof(char *));
	for (i = 0; i < im->sy + 1; i++) {
		pts[i] = (char *) ecalloc(im->sx + 1, sizeof(char));
	}

	stack = (struct seg *)safe_emalloc(sizeof(struct seg), ((int)(im->sy*im->sx)/4), 1);
	sp = stack;

	oc = gdImageGetPixel(im, x, y);

	/* required! */
	FILL_PUSH(y,x,x,1);
	/* seed segment (popped 1st) */
 	FILL_PUSH(y+1, x, x, -1);
	while (sp>stack) {
		FILL_POP(y, x1, x2, dy);
		for (x=x1; x>=0 && (!pts[y][x] && gdImageGetPixel(im,x,y)==oc); x--) {
			nc = gdImageTileGet(im,x,y);
			pts[y][x] = 1;
			gdImageSetPixel(im,x, y, nc);
		}
		if (x>=x1) {
			goto skip;
		}
		l = x+1;

		/* leak on left? */
		if (l<x1) {
			FILL_PUSH(y, l, x1-1, -dy);
		}
		x = x1+1;
		do {
			for(; x<wx2 && (!pts[y][x] && gdImageGetPixel(im,x, y)==oc); x++) {
				nc = gdImageTileGet(im,x,y);
				pts[y][x] = 1;
				gdImageSetPixel(im, x, y, nc);
			}
			FILL_PUSH(y, l, x-1, dy);
			/* leak on right? */
			if (x>x2+1) {
				FILL_PUSH(y, x2+1, x-1, -dy);
			}
skip:		for(x++; x<=x2 && (pts[y][x] || gdImageGetPixel(im,x, y)!=oc); x++);
			l = x;
		} while (x<=x2);
	}

	for(i = 0; i < im->sy + 1; i++) {
		efree(pts[i]);
	}

	efree(pts);
	efree(stack);
}