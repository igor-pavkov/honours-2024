static void star_vertices (pointf* vertices, pointf* bb)
{
    int i;
    pointf sz = *bb;
    double offset, a, aspect = (1 + sin(alpha3))/(2*cos(alpha));
    double r, r0, theta = alpha;

    /* Scale up width or height to required aspect ratio */
    a = sz.y/sz.x;
    if (a > aspect) {
	sz.x = sz.y/aspect;
    }
    else if (a < aspect) {
	sz.y = sz.x*aspect;
    }

    /* for given sz, get radius */
    r = sz.x/(2*cos(alpha));
    r0 = (r*cos(alpha)*cos(alpha4))/(sin(alpha4)*cos(alpha2));
    
    /* offset is the y shift of circle center from bb center */
    offset = (r*(1 - sin(alpha3)))/2;

    for (i = 0; i < 10; i += 2) {
	vertices[i].x = r*cos(theta);
	vertices[i].y = r*sin(theta) - offset;
	theta += alpha2;
	vertices[i+1].x = r0*cos(theta);
	vertices[i+1].y = r0*sin(theta) - offset;
	theta += alpha2;
    }

    *bb = sz;
}