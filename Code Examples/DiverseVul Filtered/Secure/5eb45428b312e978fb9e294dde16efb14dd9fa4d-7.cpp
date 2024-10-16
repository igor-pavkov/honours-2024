static void usage(char *me, bool fail)
{
	fprintf(stderr, "Usage: %s create {lxcpath} {name} {pid} {type} "
			"{bridge} {nicname}\n", me);
	fprintf(stderr, "Usage: %s delete {lxcpath} {name} "
			"{/proc/<pid>/ns/net} {type} {bridge} {nicname}\n", me);
	fprintf(stderr, "{nicname} is the name to use inside the container\n");

	if (fail)
		exit(EXIT_FAILURE);

	exit(EXIT_SUCCESS);
}