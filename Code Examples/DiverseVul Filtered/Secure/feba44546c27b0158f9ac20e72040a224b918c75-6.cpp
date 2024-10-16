int gdImageSmooth(gdImagePtr im, float weight)
{
	float filter[3][3] =	{{1.0,1.0,1.0},
				{1.0,0.0,1.0},
				{1.0,1.0,1.0}};
	
	filter[1][1] = weight;
	
	return gdImageConvolution(im, filter, weight+8, 0);
}