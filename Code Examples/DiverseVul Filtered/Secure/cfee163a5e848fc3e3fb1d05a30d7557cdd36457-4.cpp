void _gdImageFillTiled(gdImagePtr im, int x, int y, int nc)
{
	int i,l, x1, x2, dy;
	int oc;   /* old pixel value */
	int tiled;
	int wx2,wy2;
	/* stack of filled segments */
	struct seg *stack;
	struct seg *sp;

	int **pts;
	if(!im->tile){
		return;
	}

	wx2=im->sx;wy2=im->sy;
	tiled = nc==gdTiled;

	nc =  gdImageTileGet(im,x,y);
	pts = (int **) gdCalloc(sizeof(int *) * im->sy, sizeof(int));

	for (i=0; i<im->sy;i++) {
		pts[i] = (int *) gdCalloc(im->sx, sizeof(int));
	}

	stack = (struct seg *)gdMalloc(sizeof(struct seg) * ((int)(im->sy*im->sx)/4));
	sp = stack;

	oc = gdImageGetPixel(im, x, y);

	/* required! */
	FILL_PUSH(y,x,x,1);
	/* seed segment (popped 1st) */
 	FILL_PUSH(y+1, x, x, -1);
	while (sp>stack) {
		FILL_POP(y, x1, x2, dy);
		for (x=x1; x>=0 && (!pts[y][x] && gdImageGetPixel(im,x,y)==oc); x--) {
			if (pts[y][x]){
				/* we should never be here */
				break;
			}
			nc = gdImageTileGet(im,x,y);
			pts[y][x]=1;
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
			for (; x<=wx2 && (!pts[y][x] && gdImageGetPixel(im,x, y)==oc) ; x++) {
				if (pts[y][x]){
					/* we should never be here */
					break;
				}
				nc = gdImageTileGet(im,x,y);
				pts[y][x]=1;
				gdImageSetPixel(im, x, y, nc);
			}
			FILL_PUSH(y, l, x-1, dy);
			/* leak on right? */
			if (x>x2+1) {
				FILL_PUSH(y, x2+1, x-1, -dy);
			}
skip:			for (x++; x<=x2 && (pts[y][x] || gdImageGetPixel(im,x, y)!=oc); x++);
			l = x;
		} while (x<=x2);
	}
	for (i=0; i<im->sy;i++) {
		gdFree(pts[i]);
	}
	gdFree(pts);
	gdFree(stack);
}