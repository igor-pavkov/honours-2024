static double quant(double val, double q)
{
    int i;
    i = val / q;
    if (i * q + .00001 < val)
	i++;
    return i * q;
}