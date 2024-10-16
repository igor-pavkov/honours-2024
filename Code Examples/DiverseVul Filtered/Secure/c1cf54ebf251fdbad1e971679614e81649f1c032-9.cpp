int randseed(bool srand_it)
{
	/*
	   srand pre-seed function based on /dev/urandom
	   */
	unsigned int seed = time(NULL) + getpid();

	FILE *f;
	f = fopen("/dev/urandom", "r");
	if (f) {
		int ret = fread(&seed, sizeof(seed), 1, f);
		if (ret != 1)
			SYSDEBUG("unable to fread /dev/urandom, fallback to time+pid rand seed");

		fclose(f);
	}

	if (srand_it)
		srand(seed);

	return seed;
}