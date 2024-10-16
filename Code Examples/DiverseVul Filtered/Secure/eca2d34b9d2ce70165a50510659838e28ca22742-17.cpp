static u32 interpolate_value(u32 value, const struct linear_segments *segments,
			     unsigned len)
{
	u64 tmp64;
	u32 dx, dy;
	int i, ret;

	if (value >= segments[0].x)
		return segments[0].y;
	if (value < segments[len-1].x)
		return segments[len-1].y;

	for (i = 1; i < len - 1; i++) {
		/* If value is identical, no need to interpolate */
		if (value == segments[i].x)
			return segments[i].y;
		if (value > segments[i].x)
			break;
	}

	/* Linear interpolation between the two (x,y) points */
	dy = segments[i].y - segments[i - 1].y;
	dx = segments[i - 1].x - segments[i].x;
	tmp64 = value - segments[i].x;
	tmp64 *= dy;
	do_div(tmp64, dx);
	ret = segments[i].y - tmp64;

	return ret;
}