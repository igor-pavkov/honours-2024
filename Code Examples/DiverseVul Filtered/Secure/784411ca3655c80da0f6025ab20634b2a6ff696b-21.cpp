static void unrecognized(node_t * n, char *p)
{
    agerr(AGWARN, "node %s, port %s unrecognized\n", agnameof(n), p);
}