void gdImageAntialias (gdImagePtr im, int antialias)
{
	if (im->trueColor){
		im->antialias = antialias;
	}
}