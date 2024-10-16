static int stylenode(GVJ_t * job, node_t * n)
{
    char **pstyle, *s;
    int istyle;
    double penwidth;

    if ((pstyle = checkStyle(n, &istyle)))
	gvrender_set_style(job, pstyle);

    if (N_penwidth && ((s = agxget(n, N_penwidth)) && s[0])) {
	penwidth = late_double(n, N_penwidth, 1.0, 0.0);
	gvrender_set_penwidth(job, penwidth);
    }

    return istyle;
}