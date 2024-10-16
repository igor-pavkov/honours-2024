static void prbox(boxf b)
{
    fprintf(stderr, "((%.5g,%.5g),(%.5g,%.5g))\n", b.LL.x, b.LL.y, b.UR.x,
	    b.UR.y);
}