static unsigned int matchpoint(unsigned int side, unsigned int *x1, unsigned int *y1, unsigned int *avg1, const unsigned int *x2, const unsigned int *y2, const unsigned int *avg2, unsigned int max) {
    unsigned int i, j, best, match = 0, ksize = side / 4;

    for(i=0; i<3; i++) {
	best = 0;
	for(j=0; j<3; j++) {
	    /* approximately measure the distance from the best matching reference - avoid N*N total war */
	    int diffx = (int)x1[i] - (int)x2[j];
	    int diffy = ((int)y1[i] - (int)y2[j]);
	    unsigned int diff = sqrt(diffx*diffx + diffy*diffy);
	    if(diff > ksize * 3 / 4 || (unsigned int)abs((int)avg1[i]-(int)avg2[j]) > max / 5)
		continue;
	    
	    diff = 100 - diff * 60 / (ksize * 3 / 4);
	    if(diff > best)
		best = diff;
	}
	match += best;
    }
    return match / 3;
}