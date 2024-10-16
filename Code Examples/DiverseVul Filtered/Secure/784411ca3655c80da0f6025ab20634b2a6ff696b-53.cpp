char* penColor(GVJ_t * job, node_t * n)
{
    char *color;

    color = late_nnstring(n, N_color, "");
    if (!color[0])
	color = DEFAULT_COLOR;
    gvrender_set_pencolor(job, color);
    return color;
}