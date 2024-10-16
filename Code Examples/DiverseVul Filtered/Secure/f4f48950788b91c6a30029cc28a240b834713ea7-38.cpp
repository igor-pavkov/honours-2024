static void get_base_point(DBBox *bbox, int alignment, double *bx, double *by)
{
    const int halign = alignment & 3;
    const int valign = alignment & 12;
    if (bx)
        switch (halign) {
        case HALIGN_LEFT:
            *bx = bbox->xMin;
            break;
        case HALIGN_CENTER:
            *bx = (bbox->xMax + bbox->xMin) / 2.0;
            break;
        case HALIGN_RIGHT:
            *bx = bbox->xMax;
            break;
        }
    if (by)
        switch (valign) {
        case VALIGN_TOP:
            *by = bbox->yMin;
            break;
        case VALIGN_CENTER:
            *by = (bbox->yMax + bbox->yMin) / 2.0;
            break;
        case VALIGN_SUB:
            *by = bbox->yMax;
            break;
        }
}