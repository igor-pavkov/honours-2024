static pointf star_size (pointf sz0)
{
    pointf sz;
    double r0, r, rx, ry;

    rx = sz0.x/(2*cos(alpha));
    ry = sz0.y/(sin(alpha) + sin(alpha3));
    r0 = MAX(rx,ry);
    r = (r0*sin(alpha4)*cos(alpha2))/(cos(alpha)*cos(alpha4));

    sz.x = 2*r*cos(alpha);
    sz.y = r*(1 + sin(alpha3));
    return sz;
}