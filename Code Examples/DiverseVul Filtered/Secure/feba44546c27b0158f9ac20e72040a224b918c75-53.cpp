int gdImageGaussianBlur(gdImagePtr im)
{
	float filter[3][3] =	{{1.0,2.0,1.0},
				{2.0,4.0,2.0},
				{1.0,2.0,1.0}};
	
	return gdImageConvolution(im, filter, 16, 0); 
}