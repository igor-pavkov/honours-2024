static int gdColorMatch(gdImagePtr im, int col1, int col2, float threshold)
{
	const int dr = gdImageRed(im, col1) - gdImageRed(im, col2);
	const int dg = gdImageGreen(im, col1) - gdImageGreen(im, col2);
	const int db = gdImageBlue(im, col1) - gdImageBlue(im, col2);
	const int da = gdImageAlpha(im, col1) - gdImageAlpha(im, col2);
	const double dist = sqrt(dr * dr + dg * dg + db * db + da * da);
	const double dist_perc = sqrt(dist / (255^2 + 255^2 + 255^2));
	return (dist_perc <= threshold);
	//return (100.0 * dist / 195075) < threshold;
}