static double line_length(double x0, double y0, double x1, double y1)
{
	double dx = x0 - x1;
	double dy = y0 - y1;

	return hypot(dx, dy);
}