static int boundary_sub(int a, int adir,
			int b, int bdir,
			int *c, int *cdir)
{
	adir = adir < 0 ? -1 : (adir > 0 ? 1 : 0);
	bdir = bdir < 0 ? -1 : (bdir > 0 ? 1 : 0);
	*c = a - b;
	*cdir = adir - bdir;
	if (*cdir == -2) {
		(*c)--;
	} else if (*cdir == 2) {
		(*c)++;
	}
	return 0;
}