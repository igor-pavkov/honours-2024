static int gdAlphaOverlayColor (int src, int dst, int max )
{
	/* this function implements the algorithm
	 *
	 * for dst[rgb] < 0.5,
	 *   c[rgb] = 2.src[rgb].dst[rgb]
	 * and for dst[rgb] > 0.5,
	 *   c[rgb] = -2.src[rgb].dst[rgb] + 2.dst[rgb] + 2.src[rgb] - 1
	 *
	 */

	dst = dst << 1;
	if( dst > max ) {
		/* in the "light" zone */
		return dst + (src << 1) - (dst * src / max) - max;
	} else {
		/* in the "dark" zone */
		return dst * src / max;
	}
}