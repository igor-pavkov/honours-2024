static double userSize(node_t * n)
{
    double w, h;
    w = late_double(n, N_width, 0.0, MIN_NODEWIDTH);
    h = late_double(n, N_height, 0.0, MIN_NODEHEIGHT);
    return POINTS(MAX(w, h));
}