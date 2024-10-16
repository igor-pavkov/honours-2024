int gdImageMeanRemoval(gdImagePtr im)
{
	float filter[3][3] =	{{-1.0,-1.0,-1.0},
				{-1.0,9.0,-1.0},
				{-1.0,-1.0,-1.0}};
	
	return gdImageConvolution(im, filter, 1, 0);
}