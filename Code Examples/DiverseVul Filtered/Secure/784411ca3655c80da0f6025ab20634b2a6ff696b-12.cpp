static void cylinder_draw(GVJ_t * job, pointf * AF, int sides, int style, int filled)
{
    pointf vertices[7];
    double y0 = AF[0].y;
    double y02 = y0+y0;

    vertices[0] = AF[0];
    vertices[1].x = AF[1].x;
    vertices[1].y = y02 - AF[1].y;
    vertices[2].x = AF[2].x;
    vertices[2].y = y02 - AF[2].y;
    vertices[3].x = AF[3].x;
    vertices[3].y = y02 - AF[3].y;
    vertices[4].x = AF[4].x;
    vertices[4].y = y02 - AF[4].y;
    vertices[5].x = AF[5].x;
    vertices[5].y = y02 - AF[5].y;
    vertices[6] = AF[6];

    gvrender_beziercurve(job, AF, sides, FALSE, FALSE, filled);
    gvrender_beziercurve(job, vertices, 7, FALSE, FALSE, FALSE);
}