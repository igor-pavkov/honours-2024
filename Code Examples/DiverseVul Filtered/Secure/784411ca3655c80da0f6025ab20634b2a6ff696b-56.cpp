static void cylinder_vertices (pointf* vertices, pointf* bb)
{
    double x = bb->x/2;
    double y = bb->y/2;
    double yr = bb->y/11;

    vertices[0].x = x;
    vertices[0].y = y-yr;
    vertices[1].x = x;
    vertices[1].y = y-(1-0.551784)*yr;
    vertices[2].x = 0.551784*x;
    vertices[2].y = y;
    vertices[3].x = 0;
    vertices[3].y = y;
    vertices[4].x = -0.551784*x;
    vertices[4].y = y;
    vertices[5].x = -x;
    vertices[5].y = vertices[1].y;
    vertices[6].x = -x;
    vertices[6].y = y-yr;
    vertices[7] = vertices[6];
    vertices[8].x = -x;
    vertices[8].y = yr-y;
    vertices[9] = vertices[8];
    vertices[10].x = -x;
    vertices[10].y = -vertices[1].y;
    vertices[11].x = vertices[4].x;
    vertices[11].y = -vertices[4].y;
    vertices[12].x = vertices[3].x;
    vertices[12].y = -vertices[3].y;
    vertices[13].x = vertices[2].x;
    vertices[13].y = -vertices[2].y;
    vertices[14].x = vertices[1].x;
    vertices[14].y = -vertices[1].y;
    vertices[15].x = vertices[0].x;
    vertices[15].y = -vertices[0].y;
    vertices[16] = vertices[15];
    vertices[18] = vertices[17] = vertices[0];
}