static void usage(void)
{
	printf("%s: [options] mountpoint\n"
	       "Options:\n"
	       " -h		    print help\n"
	       " -V		    print version\n"
	       " -o opt[,opt...]   mount options\n"
	       " -u		    unmount\n"
	       " -q		    quiet\n"
	       " -z		    lazy unmount\n",
	       progname);
	exit(1);
}