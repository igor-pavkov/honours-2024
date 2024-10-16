int gdImageEmboss(gdImagePtr im)
{
	float filter[3][3] =	{{-1.0,0.0,-1.0},
				{0.0,4.0,0.0},
				{-1.0,0.0,-1.0}};
	
	return gdImageConvolution(im, filter, 1, 127);
}