static void dashedSet (gdImagePtr im, int x, int y, int color, int *onP, int *dashStepP, int wid, int vert)
{
	int dashStep = *dashStepP;
	int on = *onP;
	int w, wstart;

	dashStep++;
	if (dashStep == gdDashSize) {
		dashStep = 0;
		on = !on;
	}
	if (on) {
		if (vert) {
			wstart = y - wid / 2;
			for (w = wstart; w < wstart + wid; w++) {
				gdImageSetPixel(im, x, w, color);
			}
		} else {
			wstart = x - wid / 2;
			for (w = wstart; w < wstart + wid; w++) {
				gdImageSetPixel(im, w, y, color);
			}
		}
	}
	*dashStepP = dashStep;
	*onP = on;
}