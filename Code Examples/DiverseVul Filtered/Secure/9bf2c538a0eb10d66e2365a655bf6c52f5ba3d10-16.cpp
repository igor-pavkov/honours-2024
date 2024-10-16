static int usage(void)
{
	fprintf(stderr, "Usage: ip netns list\n");
	fprintf(stderr, "       ip netns add NAME\n");
	fprintf(stderr, "       ip netns attach NAME PID\n");
	fprintf(stderr, "       ip netns set NAME NETNSID\n");
	fprintf(stderr, "       ip [-all] netns delete [NAME]\n");
	fprintf(stderr, "       ip netns identify [PID]\n");
	fprintf(stderr, "       ip netns pids NAME\n");
	fprintf(stderr, "       ip [-all] netns exec [NAME] cmd ...\n");
	fprintf(stderr, "       ip netns monitor\n");
	fprintf(stderr, "       ip netns list-id\n");
	fprintf(stderr, "NETNSID := auto | POSITIVE-INT\n");
	exit(-1);
}