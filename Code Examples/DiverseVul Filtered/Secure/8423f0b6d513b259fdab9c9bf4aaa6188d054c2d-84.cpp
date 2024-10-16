static int boundary_nearer(int min, int mindir,
			   int best, int bestdir,
			   int max, int maxdir)
{
	int dmin, dmindir;
	int dmax, dmaxdir;
	boundary_sub(best, bestdir, min, mindir, &dmin, &dmindir);
	boundary_sub(max, maxdir, best, bestdir, &dmax, &dmaxdir);
	return boundary_lt(dmin, dmindir, dmax, dmaxdir);
}