int gdImageEdgeDetectQuick(gdImagePtr src)
{
	float filter[3][3] =	{{1.0,1.0,1.0},
				{0.0,0.0,0.0},
				{-1.0,-1.0,-1.0}};
				
	return gdImageConvolution(src, filter, 1, 127);
}