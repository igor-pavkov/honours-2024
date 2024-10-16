static void Mcircle_hack(GVJ_t * job, node_t * n)
{
    double x, y;
    pointf AF[2], p;

    y = .7500;
    x = .6614;			/* x^2 + y^2 = 1.0 */
    p.y = y * ND_ht(n) / 2.0;
    p.x = ND_rw(n) * x;		/* assume node is symmetric */

    AF[0] = add_pointf(p, ND_coord(n));
    AF[1].y = AF[0].y;
    AF[1].x = AF[0].x - 2 * p.x;
    gvrender_polyline(job, AF, 2);
    AF[0].y -= 2 * p.y;
    AF[1].y = AF[0].y;
    gvrender_polyline(job, AF, 2);
}