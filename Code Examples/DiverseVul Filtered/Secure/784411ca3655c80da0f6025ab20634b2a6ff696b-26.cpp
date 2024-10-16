static int poly_path(node_t * n, port * p, int side, boxf rv[], int *kptr)
{
    side = 0;

    if (ND_label(n)->html && ND_has_port(n)) {
	side = html_path(n, p, side, rv, kptr);
    }
    return side;
}