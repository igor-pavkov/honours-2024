static int getmetrics(unsigned int side, unsigned int *imagedata, struct icomtr *res, const char *tempd) {
    unsigned int x, y, xk, yk, i, j, *tmp;
    unsigned int ksize = side / 4, bwonly = 0;
    unsigned int edge_avg[6], edge_x[6]={0,0,0,0,0,0}, edge_y[6]={0,0,0,0,0,0}, noedge_avg[6], noedge_x[6]={0,0,0,0,0,0}, noedge_y[6]={0,0,0,0,0,0};
    double *sobel;

    if(!(tmp = cli_malloc(side*side*4*2)))
	return CL_EMEM;

    memset(res, 0, sizeof(*res));

    /* compute colored, gray, bright and dark areas, color presence */
    for(y=0; y<=side - ksize; y++) {
	for(x=0; x<=side - ksize; x++) {
	    unsigned int colsum = 0, lightsum = 0;
	    unsigned int r, g, b, s, v, delta;

	    if(x==0 && y==0) {
		/* Here we handle the 1st window which is fully calculated */
		for(yk=0; yk<ksize; yk++) {
		    for(xk=0; xk<ksize; xk++) {
			hsv(imagedata[yk * side + xk], &r, &g, &b, &s, &v, &delta);
			colsum += (unsigned int)sqrt(s*s*v);
			lightsum += v;

			/* count colors (full square) */
			if(s> 85 && v> 85) {
			    res->ccount++;
			    res->rsum += 100 - 100 * abs((int)g - (int)b) / delta;
			    res->gsum += 100 - 100 * abs((int)r - (int)b) / delta;
			    res->bsum += 100 - 100 * abs((int)r - (int)g) / delta;
			}
		    }
		}
	    } else if(x) { /* Here we incrementally calculate rows and columns
			      code is split as gcc produces faster code this way */
		colsum = tmp[y*side+x-1];
		lightsum = tmp[side*side + y*side+x-1];
		for(yk=0; yk<ksize; yk++) {
		    /* remove previous column */ 
		    hsv(imagedata[(y+yk) * side + x-1], &r, &g, &b, &s, &v, &delta);
		    colsum -= (unsigned int)sqrt(s*s*v);
		    lightsum -= v;
		    /* add next column */
		    hsv(imagedata[(y+yk) * side + x+ksize-1], &r, &g, &b, &s, &v, &delta);
		    colsum += (unsigned int)sqrt(s*s*v);
		    lightsum += v;

		    /* count colors (full column or only the last px) */
		    if((y == 0 || yk==ksize-1) && s> 85 && v> 85) {
		    	res->ccount++;
		    	res->rsum += 100 - 100 * abs((int)g - (int)b) / delta;
		    	res->gsum += 100 - 100 * abs((int)r - (int)b) / delta;
		    	res->bsum += 100 - 100 * abs((int)r - (int)g) / delta;
		    }
		}
	    } else {
		colsum = tmp[(y-1)*side];
		lightsum = tmp[side*side + (y-1)*side];
		for(xk=0; xk<ksize; xk++) {
		    /* remove previous row */
		    hsv(imagedata[(y-1) * side + xk], &r, &g, &b, &s, &v, &delta);
		    colsum -= (unsigned int)sqrt(s*s*v);
		    lightsum -= v;

		    /* add next row */
		    hsv(imagedata[(y+ksize-1) * side + xk], &r, &g, &b, &s, &v, &delta);
		    colsum += (unsigned int)sqrt(s*s*v);
		    lightsum += v;

		    /* count colors (full row) */
		    if(s> 85 && v> 85) {
		    	res->ccount++;
		    	res->rsum += 100 - 100 * abs((int)g - (int)b) / delta;
		    	res->gsum += 100 - 100 * abs((int)r - (int)b) / delta;
		    	res->bsum += 100 - 100 * abs((int)r - (int)g) / delta;
		    }
		}
	    }
	    tmp[y*side+x] = colsum;
	    tmp[side*side + y*side+x] = lightsum;
	}
    }


    /* extract top 3 non overlapping areas for: colored, gray, bright and dark areas, color presence */
    for(i=0; i<3; i++) {
	res->gray_avg[i] = 0xffffffff;
	res->dark_avg[i] = 0xffffffff;
	for(y=0; y<side - ksize; y++) {
	    for(x=0; x<side-1 - ksize; x++) {
		unsigned int colsum = tmp[y*side+x], lightsum = tmp[side*side + y*side+x];

		if(colsum > res->color_avg[i]) {
		    for(j=0; j<i; j++) {
			if(x + ksize > res->color_x[j] && x < res->color_x[j] + ksize &&
			   y + ksize > res->color_y[j] && y < res->color_y[j] + ksize) break;
		    }
		    if(j==i) {
			res->color_avg[i] = colsum;
			res->color_x[i] = x;
			res->color_y[i] = y;
		    }
		}
		if(colsum < res->gray_avg[i]) {
		    for(j=0; j<i; j++) {
			if(x + ksize > res->gray_x[j] && x < res->gray_x[j] + ksize &&
			   y + ksize > res->gray_y[j] && y < res->gray_y[j] + ksize) break;
		    }
		    if(j==i) {
			res->gray_avg[i] = colsum;
			res->gray_x[i] = x;
			res->gray_y[i] = y;
		    }
		}
		if(lightsum > res->bright_avg[i]) {
		    for(j=0; j<i; j++) {
			if(x + ksize > res->bright_x[j] && x < res->bright_x[j] + ksize &&
			   y + ksize > res->bright_y[j] && y < res->bright_y[j] + ksize) break;
		    }
		    if(j==i) {
			res->bright_avg[i] = lightsum;
			res->bright_x[i] = x;
			res->bright_y[i] = y;
		    }
		}
		if(lightsum < res->dark_avg[i]) {
		    for(j=0; j<i; j++) {
			if(x + ksize > res->dark_x[j] && x < res->dark_x[j] + ksize &&
			   y + ksize > res->dark_y[j] && y < res->dark_y[j] + ksize) break;
		    }
		    if(j==i) {
			res->dark_avg[i] = lightsum;
			res->dark_x[i] = x;
			res->dark_y[i] = y;
		    }
		}
	    }
	}
    }

    /* abs->avg */
    for(i=0; i<3; i++) {
	res->color_avg[i] /= ksize*ksize;
	res->gray_avg[i] /= ksize*ksize;
	res->bright_avg[i] /= ksize*ksize;
	res->dark_avg[i] /= ksize*ksize;
    }

    if(res->ccount * 100 / side / side > 5) {
	res->rsum /= res->ccount;
	res->gsum /= res->ccount;
	res->bsum /= res->ccount;
	res->ccount = res->ccount * 100 / side / side;
    } else {
	res->ccount = 0;
	res->rsum = 0;
	res->gsum = 0;
	res->bsum = 0;
	bwonly = 1;
    }

    /* Edge detection - Sobel */

    /* Sobel 1 - gradients */
    i = 0;
#ifdef USE_FLOATS
    sobel = cli_malloc(side * side * sizeof(double));
    if(!sobel) {
	free(tmp);
	return CL_EMEM;
    }
#else
#define sobel imagedata
#endif
    for(y=0; y<side; y++) {
	for(x=0; x<side; x++) {
	    sobel[y * side + x] = LABDIFF(imagedata[y * side + x]);
	}
    }
    for(y=1; y<side-1; y++) {
	for(x=1; x<side-1; x++) {
	    unsigned int sob;
#ifdef USE_FLOATS
	    double gx, gy;
#else
	    unsigned int gx, gy;
#endif

	    /* X matrix */
	    gx =  sobel[(y-1) * side + (x-1)];
	    gx += sobel[(y+0) * side + (x-1)] * 2;
	    gx += sobel[(y+1) * side + (x-1)];
	    gx -= sobel[(y-1) * side + (x+1)];
	    gx -= sobel[(y+0) * side + (x+1)] * 2;
	    gx -= sobel[(y+1) * side + (x+1)];

	    /* Y matrix */
	    gy =  sobel[(y-1) * side + (x-1)];
	    gy += sobel[(y-1) * side + (x+0)] * 2;
	    gy += sobel[(y-1) * side + (x+1)];
	    gy -= sobel[(y+1) * side + (x-1)];
	    gy -= sobel[(y+1) * side + (x+0)] * 2;
	    gy -= sobel[(y+1) * side + (x+1)];

	    sob = (int)sqrt(gx*gx + gy*gy);
	    tmp[y * side + x] = sob;
	    if(sob > i) i = sob;
	}
    }
#ifdef USE_FLOATS
    free(sobel);
#endif

    /* Sobel 2 - norm to max */
    if(i) {
	for(y=1; y<side-1; y++) {
	    for(x=1; x<side-1; x++) {
		unsigned int c = tmp[y * side + x];
		c = c * 255 / i;
		imagedata[y * side + x] = 0xff000000 | c | (c<<8) | (c<<16);
	    }
	}
    }

    /* black borders */
    for(x=0; x<side; x++) {
	imagedata[x] = 0xff000000;
	imagedata[(side-1) * side + x] = 0xff000000;
    }
    for(y=0; y<side; y++) {
	imagedata[y * side] = 0xff000000;
	imagedata[y * side + side - 1] = 0xff000000;
    }
    makebmp("3-edge", tempd, side, side, imagedata);


    /* gaussian blur */
    for(y=1; y<side-1; y++) {
	for(x=1; x<side-1; x++) {
	    unsigned int sum=0, tot=0;
	    int disp;
	    for(disp=-MIN((int)x, gkernsz/2); disp<=MIN((int)(side-1 - x), gkernsz/2); disp++){
		unsigned int c = imagedata[y * side + x + disp] & 0xff;
		sum += c * gaussk[disp + gkernsz/2];
		tot += gaussk[disp + gkernsz/2];
	    }
	    sum /= tot;
	    imagedata[y * side + x] &= 0xff;
	    imagedata[y * side + x] |= sum<<8;
	}
    }
    i=0;
    for(y=1; y<side-1; y++) {
	for(x=1; x<side-1; x++) {
	    unsigned int sum=0, tot=0;
	    int disp;
	    for(disp=-MIN((int)y, gkernsz/2); disp<=MIN((int)(side-1 - y), gkernsz/2); disp++){
		unsigned int c = (imagedata[(y + disp) * side + x] >> 8) & 0xff;
		sum += c * gaussk[disp + gkernsz/2];
		tot += gaussk[disp + gkernsz/2];
	    }
	    sum /= tot;
	    if(sum>i) i=sum;
	    imagedata[y * side + x] = 0xff000000 | sum | (sum<<8) | (sum<<16);
	}
    }
    makebmp("4-gauss", tempd, side, side, imagedata);


    /* calculate edges */
    for(y=0; y<=side - ksize; y++) {
	for(x=0; x<=side-1 - ksize; x++) {
	    unsigned int sum = 0;

	    if(x==0 && y==0) { /* 1st windows */
		for(yk=0; yk<ksize; yk++) {
		    for(xk=0; xk<ksize; xk++)
			sum += imagedata[(y + yk) * side + x + xk] & 0xff;
		}
	    } else if(x) { /* next column */
		sum = tmp[y*side + x - 1];
		for(yk=0; yk<ksize; yk++) {
		    sum -= imagedata[(y + yk) * side + x - 1] & 0xff;
		    sum += imagedata[(y + yk) * side + x + ksize - 1] & 0xff;
		}
	    } else { /* next row */
		sum = tmp[(y-1)*side];
		for(xk=0; xk<ksize; xk++) {
		    sum -= imagedata[(y - 1) * side + xk] & 0xff;
		    sum += imagedata[(y + ksize - 1) * side + xk] & 0xff;
		}
	    }
	    tmp[y*side + x] = sum;
	}
    }

    /* calculate best and worst 3 (or 6) edged areas */
    for(i=0; i<3 * (bwonly + 1); i++) {
	edge_avg[i] = 0;
	noedge_avg[i] = 0xffffffff;
	for(y=0; y<side - ksize; y++) {
	    for(x=0; x<side-1 - ksize; x++) {
		unsigned int sum = tmp[y*side + x];

		if(sum > edge_avg[i]) {
		    for(j=0; j<i; j++) {
			if(x + ksize > edge_x[j] && x < edge_x[j] + ksize &&
			   y + ksize > edge_y[j] && y < edge_y[j] + ksize) break;
		    }
		    if(j==i) {
			edge_avg[i] = sum;
			edge_x[i] = x;
			edge_y[i] = y;
		    }
		}
		if(sum < noedge_avg[i]) {
		    for(j=0; j<i; j++) {
			if(x + ksize > noedge_x[j] && x < noedge_x[j] + ksize &&
			   y + ksize > noedge_y[j] && y < noedge_y[j] + ksize) break;
		    }
		    if(j==i) {
			noedge_avg[i] = sum;
			noedge_x[i] = x;
			noedge_y[i] = y;
		    }
		}
	    }
	}
    }

    free(tmp);

    /* abs->avg */
    for(i=0; i<3; i++) {
	res->edge_avg[i] = edge_avg[i] / ksize / ksize;
	res->edge_x[i] = edge_x[i];
	res->edge_y[i] = edge_y[i];
	res->noedge_avg[i] = noedge_avg[i] / ksize / ksize;
	res->noedge_x[i] = noedge_x[i];
	res->noedge_y[i] = noedge_y[i];
    }
    if(bwonly) {
	for(i=0; i<3; i++) {
	    res->color_avg[i] = edge_avg[i+3] / ksize / ksize;
	    res->color_x[i] = edge_x[i+3];
	    res->color_y[i] = edge_y[i+3];
	    res->gray_avg[i] = noedge_avg[i+3] / ksize / ksize;
	    res->gray_x[i] = edge_x[i+3];
	    res->gray_y[i] = edge_y[i+3];
	}
    }

    cli_dbgmsg("edge areas: %u@(%u,%u) %u@(%u,%u) %u@(%u,%u)\n", res->edge_avg[0], res->edge_x[0], res->edge_y[0], res->edge_avg[1], res->edge_x[1], res->edge_y[1], res->edge_avg[2], res->edge_x[2], res->edge_y[2]);
    cli_dbgmsg("noedge areas: %u@(%u,%u) %u@(%u,%u) %u@(%u,%u)\n", res->noedge_avg[0], res->noedge_x[0], res->noedge_y[0], res->noedge_avg[1], res->noedge_x[1], res->noedge_y[1], res->noedge_avg[2], res->noedge_x[2], res->noedge_y[2]);
    cli_dbgmsg("%s areas: %u@(%u,%u) %u@(%u,%u) %u@(%u,%u)\n", bwonly?"edge(2nd)":"color", res->color_avg[0], res->color_x[0], res->color_y[0], res->color_avg[1], res->color_x[1], res->color_y[1], res->color_avg[2], res->color_x[2], res->color_y[2]);
    cli_dbgmsg("%s areas: %u@(%u,%u) %u@(%u,%u) %u@(%u,%u)\n", bwonly?"noedge(2nd)":"gray", res->gray_avg[0], res->gray_x[0], res->gray_y[0], res->gray_avg[1], res->gray_x[1], res->gray_y[1], res->gray_avg[2], res->gray_x[2], res->gray_y[2]);
    cli_dbgmsg("bright areas: %u@(%u,%u) %u@(%u,%u) %u@(%u,%u)\n", res->bright_avg[0], res->bright_x[0], res->bright_y[0], res->bright_avg[1], res->bright_x[1], res->bright_y[1], res->bright_avg[2], res->bright_x[2], res->bright_y[2]);
    cli_dbgmsg("dark areas: %u@(%u,%u) %u@(%u,%u) %u@(%u,%u)\n", res->dark_avg[0], res->dark_x[0], res->dark_y[0], res->dark_avg[1], res->dark_x[1], res->dark_y[1], res->dark_avg[2], res->dark_x[2], res->dark_y[2]);
    if(!bwonly)
	cli_dbgmsg("color spread: %u,%u,%u %u%%\n", res->rsum, res->gsum, res->bsum, res->ccount);


    if(cli_debug_flag) {
#define ICOSIGSZ (2 + (3 + 2 + 2) * 3 * 2 + (2 + 2 + 2) * 3 * 4 + 2 + 2 + 2 + 2)
	char mstr[ICOSIGSZ + 1], *ptr = mstr;

	sprintf(ptr, "%02x", side); ptr+=2;
	for(i=0; i<3; i++) {
	    sprintf(ptr, "%03x", res->color_avg[i]); ptr+=3;
	    sprintf(ptr, "%02x", res->color_x[i]); ptr+=2;
	    sprintf(ptr, "%02x", res->color_y[i]); ptr+=2;
	}
	for(i=0; i<3; i++) {
	    sprintf(ptr, "%03x", res->gray_avg[i]); ptr+=3;
	    sprintf(ptr, "%02x", res->gray_x[i]); ptr+=2;
	    sprintf(ptr, "%02x", res->gray_y[i]); ptr+=2;
	}
	for(i=0; i<3; i++) {
	    sprintf(ptr, "%02x", res->bright_avg[i]); ptr+=2;
	    sprintf(ptr, "%02x", res->bright_x[i]); ptr+=2;
	    sprintf(ptr, "%02x", res->bright_y[i]); ptr+=2;
	}
	for(i=0; i<3; i++) {
	    sprintf(ptr, "%02x", res->dark_avg[i]); ptr+=2;
	    sprintf(ptr, "%02x", res->dark_x[i]); ptr+=2;
	    sprintf(ptr, "%02x", res->dark_y[i]); ptr+=2;
	}
	for(i=0; i<3; i++) {
	    sprintf(ptr, "%02x", res->edge_avg[i]); ptr+=2;
	    sprintf(ptr, "%02x", res->edge_x[i]); ptr+=2;
	    sprintf(ptr, "%02x", res->edge_y[i]); ptr+=2;
	}
	for(i=0; i<3; i++) {
	    sprintf(ptr, "%02x", res->noedge_avg[i]); ptr+=2;
	    sprintf(ptr, "%02x", res->noedge_x[i]); ptr+=2;
	    sprintf(ptr, "%02x", res->noedge_y[i]); ptr+=2;
	}
	sprintf(ptr, "%02x", res->rsum); ptr+=2;
	sprintf(ptr, "%02x", res->gsum); ptr+=2;
	sprintf(ptr, "%02x", res->bsum); ptr+=2;
	sprintf(ptr, "%02x", res->ccount);
	cli_dbgmsg("IDB SIGNATURE: ICON_NAME:GROUP1:GROUP2:%s\n", mstr);
    }

    return CL_CLEAN;
}