static unsigned int matchbwpoint(unsigned int side, unsigned int *x1a, unsigned int *y1a, unsigned int *avg1a, unsigned int *x1b, unsigned int *y1b, unsigned int *avg1b, const unsigned int *x2a, const unsigned int *y2a, const unsigned int *avg2a, const unsigned int *x2b, const unsigned int *y2b, const unsigned int *avg2b) {
    unsigned int i, j, best, match = 0, ksize = side / 4;
    unsigned int x1[6], y1[6], avg1[6], x2[6], y2[6], avg2[6];

    for(i=0; i<3; i++) {
	x1[i] = x1a[i];
	y1[i] = y1a[i];
	avg1[i] = avg1a[i];
	x2[i] = x2a[i];
	y2[i] = y2a[i];
	avg2[i] = avg2a[i];

	x1[i+3] = x1b[i];
	y1[i+3] = y1b[i];
	avg1[i+3] = avg1b[i];
	x2[i+3] = x2b[i];
	y2[i+3] = y2b[i];
	avg2[i+3] = avg2b[i];
    }

    for(i=0; i<6; i++) {
	best = 0;
	for(j=0; j<6; j++) {
	    /* approximately measure the distance from the best matching reference - avoid N*N total war */
	    int diffx = (int)x1[i] - (int)x2[j];
	    int diffy = ((int)y1[i] - (int)y2[j]);
	    unsigned int diff = sqrt(diffx*diffx + diffy*diffy);
	    if(diff > ksize * 3 / 4 || (unsigned int)abs((int)avg1[i]-(int)avg2[j]) > 255 / 5)
		continue;

	    diff = 100 - diff * 60 / (ksize * 3 / 4);
	    if(diff > best)
		best = diff;
	}
	match += best;
    }
    return match / 6;
}