static void gdImageAntiAliasedApply (gdImagePtr im, int px, int py)
{
	float p_dist, p_alpha;
	unsigned char opacity;

	/*
	 * Find the perpendicular distance from point C (px, py) to the line
	 * segment AB that is being drawn.  (Adapted from an algorithm from the
	 * comp.graphics.algorithms FAQ.)
	 */

	int LAC_2, LBC_2;

	int Ax_Cx = im->AAL_x1 - px;
	int Ay_Cy = im->AAL_y1 - py;

	int Bx_Cx = im->AAL_x2 - px;
	int By_Cy = im->AAL_y2 - py;

	/* 2.0.13: bounds check! AA_opacity is just as capable of
	 * overflowing as the main pixel array. Arne Jorgensen.
	 * 2.0.14: typo fixed. 2.0.15: moved down below declarations
	 * to satisfy non-C++ compilers.
	 */
	if (!gdImageBoundsSafe(im, px, py)) {
		return;
	}

	/* Get the squares of the lengths of the segemnts AC and BC. */
	LAC_2 = (Ax_Cx * Ax_Cx) + (Ay_Cy * Ay_Cy);
	LBC_2 = (Bx_Cx * Bx_Cx) + (By_Cy * By_Cy);

	if (((im->AAL_LAB_2 + LAC_2) >= LBC_2) && ((im->AAL_LAB_2 + LBC_2) >= LAC_2)) {
		/* The two angles are acute.  The point lies inside the portion of the
		 * plane spanned by the line segment.
		 */
		p_dist = fabs ((float) ((Ay_Cy * im->AAL_Bx_Ax) - (Ax_Cx * im->AAL_By_Ay)) / im->AAL_LAB);
	} else {
		/* The point is past an end of the line segment.  It's length from the
		 * segment is the shorter of the lengths from the endpoints, but call
		 * the distance -1, so as not to compute the alpha nor draw the pixel.
		 */
		p_dist = -1;
	}

	if ((p_dist >= 0) && (p_dist <= (float) (im->thick))) {
		p_alpha = pow (1.0 - (p_dist / 1.5), 2);

		if (p_alpha > 0) {
			if (p_alpha >= 1) {
				opacity = 255;
			} else {
				opacity = (unsigned char) (p_alpha * 255.0);
			}
			if (!im->AA_polygon || (im->AA_opacity[py][px] < opacity)) {
				im->AA_opacity[py][px] = opacity;
			}
		}
	}
}