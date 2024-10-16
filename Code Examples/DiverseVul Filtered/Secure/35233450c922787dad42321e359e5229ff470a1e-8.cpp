int test_multi_curves(void) {
    spiro_cp **spiro = NULL;
    spiro_cp *temp;
    int *pk, **nextknot = NULL;
    int *scl = NULL;
    test_bezctx **bc;
    int i, j, k, l, ret;
    double x, y;
    char ty;

    /* our simple curve test-check breaks-down if we go more than */
    /* 10x larger curves due to rounding errors on double values, */
    /* so, we either need a more complex curve test-check at end, */
    /* or we can cleverly increase in increments of "1/S_TESTS".  */
#define S_TESTS S_TESTP*5

#ifdef HAVE_PTHREADS
    pthread_attr_t tattr;
    pthread_t curve_test[S_TESTS];
    pthread_pcurve pdata[S_TESTS];

    printf("---\nMulti-thread testing of libspiro.\n");
#else
    printf("---\nSequential tests of libspiro.\n");
#endif

    ret = -1;	/* return error if out of memory */


    /* Expect lots of results, therefore create available memory. */
    /* This way, we won't be wasting time doing malloc because we */
    /* really want to shoot a whole bunch of pthreads all at once.*/
    if ( (bc=(test_bezctx**)calloc(S_TESTS,sizeof(test_bezctx*)))==NULL )
	goto test_multi_curves_exit;
    for (i=0; i < S_TESTS; i++) {
	if ( (bc[i]=(test_bezctx*)malloc(sizeof(test_bezctx)))==NULL )
	    goto test_multi_curves_exit;
	bc[i]->base.moveto = test_s_moveto;
	bc[i]->base.lineto = test_s_lineto;
	bc[i]->base.quadto = test_s_quadto;
	bc[i]->base.curveto = test_s_curveto;
	bc[i]->base.mark_knot = test_s_mark_knot;
	if ( (bc[i]->my_curve=(my_curve_data*)calloc(S_RESULTS,sizeof(my_curve_data)))==NULL )
	    goto test_multi_curves_exit;
	bc[i]->len = 0;		/* no curve yet, len=0 */
	bc[i]->is_open = 0;
	bc[i]->c_id = i;	/* identify each curve */
    }

    if ( (scl=(int*)malloc(S_TESTS*sizeof(int)))==NULL || \
	 (spiro=(spiro_cp**)calloc(S_TESTS,sizeof(spiro_cp*)))==NULL || \
	 (nextknot=(int**)calloc(S_TESTS,sizeof(int*)))==NULL )
	goto test_multi_curves_exit;
    for (i=0; i < S_TESTS; ) {
	/* NOTE: S_TESTS has to be multiple of 5 here. */
	/* because we test with path[0/1/2/5/6]tables, */
	/* ...and path[3] is used to test NOT success, */
	/* ...and path[4] is too fast @ 4 interations, */
	/* ...and path[7/8] ah complicates node check. */
	if ( (spiro[i]=malloc(cl[0]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[0],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i],nextknot[i],0);
	scl[i++]=cl[0];
	if ( (spiro[i]=malloc(cl[1]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[1],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i],nextknot[i],1);
	scl[i++]=cl[1];
	if ( (spiro[i]=malloc(cl[2]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[2],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i],nextknot[i],2);
	scl[i++]=cl[2];
	if ( (spiro[i]=malloc(cl[5]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[5],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i],nextknot[i],5);
	scl[i++]=cl[5];
	if ( (spiro[i]=malloc(cl[6]*sizeof(spiro_cp)))==NULL || \
	      (nextknot[i]=calloc(cl[6],sizeof(int)))==NULL )
	    goto test_multi_curves_exit;
	load_test_curve(spiro[i],nextknot[i],6);
	scl[i++]=cl[6];
    }

    /* Change to different sizes to make sure no duplicates */
    /* ...to verify we do not overwrite different user data */
    /* while running multiple threads all at the same time. */
    for (i=0; i < S_TESTS; i++) {
	temp = spiro[i];
	for (j=0; j < scl[i]; j++) {
	    if ( i&1 ) {
		temp[j].x = temp[j].x * (i/S_TESTS+1) + i;
		temp[j].y = temp[j].y * (i/S_TESTS+1) + i;
	    } else {
		/* Scaling bug fixed. Scale & shift at will */
		temp[j].x *= (i+1); temp[j].x += i*1000. - 5000.;
		temp[j].y *= (i+1); temp[j].y += i*3000. - 9000.;
	    }
	}
    }

    --ret;

#ifdef HAVE_PTHREADS
    /* Data and memory prepared before Pthreads.  Ready? Set? GO! */
    /* Test all curves, all at same time, wait for all to finish. */
    /* This test could fail if we had globally set variables that */
    /* could affect other functions, eg: static n=4 was moved out */
    /* into passed variable so that one user won't affect others. */
    /* GEGL is a good example of multiple users all at same time. */
    for (i=0; i < S_TESTS; i++) {
	pdata[i].spiro = spiro[i];
	pdata[i].bc = (bezctx*)(bc[i]);
	pdata[i].ret = i;
    }

    j=0;
    for (k=0; k < S_TESTS;) {
	/* Initialize and set thread joinable attribute */
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_JOINABLE);

	/* Some processors can't do too many pthreads at once so then */
	/* we need to run threads in batches until completing S_TESTS */
	for (i=k; i < S_TESTS-1; i++) {
	    /* all values passed are joined at "->" (should be okay). */
	    if ( pthread_create(&curve_test[i],&tattr,test_a_curve,(void *)&pdata[i]) ) {
		if ( i-k < 20 ) {
		    printf("bad pthread_create[%d]\n",i); /* not many */
		    j=-1;
		}
		break;
	    }
	}
	pthread_attr_destroy(&tattr);	/* Free thread attribute */
	if ( j!=-1 ) {
	    /* Test another curve while waiting for threads to finish */
	    pdata[i].ret = TaggedSpiroCPsToBezier2(pdata[i].spiro,0,pdata[i].bc);
	    printf("running simultaneous threads[%d..%d]\n",k,i);
	}
	l=i;
	while (--i >= k)
	    if ( pthread_join(curve_test[i],NULL) ) {
		fprintf(stderr,"bad pthread_join[%d]\n",i);
		j=-1;
	    }
	k=++l;
	if (j) goto test_multi_curves_exit;
    }

    for (i=0; i < S_TESTS; i++)
	if ( pdata[i].ret!=1 ) {
	    ret=ret-i;
	    fprintf(stderr,"error %d with TaggedSpiroCPsToBezier2() using data=%d.\n",ret,i);
	    goto test_multi_curves_exit;
	}
    /* All threads returned okay, Now, go check all data is good. */
#else
    /* No pthreads.h, test all curves sequentially, one at a time */
    /* Just do a math check and leave the pthread check for other */
    /* operating systems to verify libspiro has no static values. */
    for (i=0; i < S_TESTS; i++) {
	if ( TaggedSpiroCPsToBezier0(spiro[i],(bezctx*)(bc[i]))!=1 ) {
	    ret=ret-i;
	    fprintf(stderr,"error %d with TaggedSpiroCPsToBezier0() using data=%d.\n",ret,i);
	    goto test_multi_curves_exit;
	}
    }
#endif
    ret=ret-S_TESTS;

    /* Check ending x,y points versus input spiro knot locations. */
    for (i=0; i < S_TESTS; i++) {
	temp = spiro[i];
	pk = nextknot[i];
	k=0;
#ifdef VERBOSE
	printf("test[%d], input spiro[0..%d], output bc->my_curve[0..%d]\n", \
		   i, scl[i]-1, bc[i]->len-1);
#endif
	for (j=l=0; j < scl[i] && temp[j].ty!='z'; j++,l++) {
	    if (temp[j].ty=='h') {
#ifdef VERBOSE
		printf("  s[%d][ty=%c x=%g y=%g] is handle 'h' to anchor 'a'.\n", \
			l-1,temp[j].ty,temp[j].x,temp[j].y);
#endif
		--l;
	    } else {
		ty = bc[i]->my_curve[k].ty;
		x  = bc[i]->my_curve[k].x1;
		y  = bc[i]->my_curve[k].y1;
		if ( ty=='q' ) {
		    x = bc[i]->my_curve[k].x2;
		    y = bc[i]->my_curve[k].y2;
		}
		if ( ty=='c' ) {
		    x = bc[i]->my_curve[k].x3;
		    y = bc[i]->my_curve[k].y3;
		}
#ifdef VERBOSE
		printf("  s[%d][ty=%c x=%g y=%g], pk=%d mc[%d][x=%g y=%g]\n", \
			l,temp[j].ty,temp[j].x,temp[j].y,pk[j],k,x,y);
#endif
		if ( (fabs(temp[j].x - x) > 1e-5) || (fabs(temp[j].y - y) > 1e-5) ) {
		    /* close-enough for testing 1..10000 value ranges */
		    if ( j == scl[i]-2 && temp[j+1].ty=='z' )
			; /* Exception: skip test of this code point. */
			/* x and/or y are not valid for 'this' check, */
			/* If ending in 'z', then prior code point is */
			/* changed to a curve end point, which is '}' */
		    else {
			ret=ret-i;
			fprintf(stderr,"error %d with test_multi_curves() using data %d\n",ret,i);
			goto test_multi_curves_exit;
		    }
		}
		k += pk[l]+1;
	    }
	}
    }

#ifdef HAVE_PTHREADS
    printf("Multi-thread testing of libspiro passed.\n");
#else
    printf("Sequential tests of libspiro passed.\n");
#endif
    ret = 0;

test_multi_curves_exit:
    if ( nextknot!=NULL ) for (i=0; i < S_TESTS; i++) free(nextknot[i]);
    if ( spiro!=NULL ) for (i=0; i < S_TESTS; i++) free(spiro[i]);
    free(nextknot); free(spiro); free(scl);
    if ( bc!=NULL ) for (i=0; i < S_TESTS; i++) {
	free(bc[i]->my_curve); free(bc[i]);
    }
    free(bc);
    return ret;
}