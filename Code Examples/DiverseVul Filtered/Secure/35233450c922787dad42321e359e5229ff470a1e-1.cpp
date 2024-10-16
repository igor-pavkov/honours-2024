void load_test_curve(spiro_cp *spiro, int *nextknot, int c) {
/* load a test curve (and nextknot locations) into memory */
    spiro_cp path0[] = { /* ...came with unit-test */
	{334, 117, 'v'},
	{305, 176, 'v'},
	{212, 142, 'c'},
	{159, 171, 'c'},
	{224, 237, 'c'},
	{347, 335, 'c'},
	{202, 467, 'c'},
	{ 81, 429, 'v'},
	{114, 368, 'v'},
	{201, 402, 'c'},
	{276, 369, 'c'},
	{218, 308, 'c'},
	{ 91, 211, 'c'},
	{124, 111, 'c'},
	{229,  82, 'c'},
	{0, 0, 'z'}
    };
    int knot0[] = {
	1, 1, 3, 3, 2, 3, 1, 1, 1, 3, 3, 2, 2, 2, 1, 1
    };
    spiro_cp path1[] = { /* do a test using "{"..."}" */
	{ 80, 738, '{'},
	{749, 540, 'o'},
	{671, 309, 'o'},
	{521, 396, 'o'},
	{377, 333, 'o'},
	{467, 231, '}'}
    };
    int knot1[] = {
	3, 4, 2, 3, 2, 0
    };
    spiro_cp path2[] = { /* this does many iterations */
	{233, 143, '{'},
	{341, 138, 'o'},
	{386,  72, 'o'},
	{444, 141, '}'}
    };
    int knot2[] = {
	2, 1, 9, 0
    };
    spiro_cp path3[] = { /* this will fail to converge */
	{233, 144, '{'}, /* will not pass (on purpose) */
	{341, 138, 'o'},
	{386,  72, 'o'},
	{443, 141, 'o'},
	{467, 231, 'o'},
	{377, 333, '}'}
    };
    int knot3[] = {
	2, 1, 9, 1, 1, 0
    };
    spiro_cp path4[] = { /* test a cyclic calculation. */
	{-100,    0, SPIRO_G4},
	{   0,  100, SPIRO_G4},
	{ 100,    0, SPIRO_G4},
	{   0, -100, SPIRO_G4}
    };
    int knot4[] = {
	2, 2, 2, 0
    };
    spiro_cp path5[] = { /* verify curve data with []. */
	{  0,   0, '{'},
	{100, 100, 'c'},
	{200, 200, '['},
	{300, 200, ']'},
	{400, 150, 'c'},
	{300, 100, '['},
	{200, 100, ']'},
	{150,  50, 'c'},
	{100,   0, '['},
	{  0,-100, ']'},
	{-50,-200, 'c'},
	{-80,-250, '}'} /* test 23 reverses this list. */
    };
    int knot5[] = {
	1, 4, 1, 3, 3, 1, 4, 2, 1, 2, 1, 0
    };
    spiro_cp path6[] = { /* verify curve data with ah. */
	{  0,   0, '{'},
	{100, 100, 'c'},
	{200, 200, 'a'},
	{300, 200, 'h'},
	{300, 150, 'c'},
	{200, 100, 'a'},
	{100, 100, 'h'},
	{150,  50, 'c'},
	{100,   0, 'a'},
	{  0,-100, 'h'},
	{ 50,-100, 'c'},
	{ 20,-150, '}'}
    };
    int knot6[] = {
	1, 4, 3, 3, 4, 2, 2, 1, 0, 0, 0, 0
    };
    spiro_cp path7[] = { /* loop stops with ah curves. */
	{  0, 200, 'o'},
	{ 50, 210, 'o'},
	{100,   0, 'a'},
	{100, -50, 'h'},
	{  0,-190, 'a'},
	{-50,-195, 'h'},
	{-90,   0, 'a'},
	{-95,  50, 'h'},/* tests 7, 8, 20, 21 end here */
	{  0,   0, 'z'} /* call_test 12 ends with ah z */
    };
    int knot7[] = {
	1, 1, 5, 5, 6, 3, 0, 0, 0
    };
    spiro_cp path10[] = { /* start loop with ah curves */
#ifndef TEST_10_11
	{100,   0, 'a'},
	{100, -50, 'h'},
	{  0,-190, 'a'},
	{-50,-195, 'h'},
	{-90,   0, 'a'},
	{-95,  50, 'h'},
	{  0, 200, 'o'},
	{ 50, 210, 'o'}
#else
	{100,   0, '['},
	{100, -50, ']'},
	{  0,-240, 'a'},
	{-50,-245, 'h'},
	{-90, -50, '['},
	{-95,   0, ']'},
	{ -5, 200, 'o'},
	{ 45, 210, 'o'}
#endif
    };
    int knot10[] = {
	5, 6, 3, 1, 1, 5, 0, 0
    };
    spiro_cp path13[] = { /* start open curve using {h */
	{100,   0, '{'},
	{100, -50, 'h'},
	{  0,-190, 'a'},
	{-50,-195, 'h'},
	{-90,   0, 'a'},
	{-95,  50, 'h'},
	{  0, 200, 'o'},
	{ 50, 210, '}'}
    };
    int knot13[] = {
	5, 6, 3, 1, 1, 5, 0, 0
    };
    spiro_cp path14[] = { /* very big path[4] version. */
	{-100e10,    0, SPIRO_G4},
	{   0,  100e10, SPIRO_G4},
	{ 100e10,    0, SPIRO_G4},
	{   0, -100e10, SPIRO_G4}
    };
    spiro_cp path15[] = { /* very tiny path[4] version */
	{-100e-42,    0, SPIRO_G4},
	{   0,  100e-42, SPIRO_G4},
	{ 100e-42,    0, SPIRO_G4},
	{   0, -100e-42, SPIRO_G4}
    };
    int i;

    /* Load static variable tables into memory because */
    /* SpiroCPsToBezier0() modifies start & end values */
    /* Later call-tests will also modify these so that */
    /* we can verify multi-threads aren't overwriting. */
    if ( c==0 ) for (i = 0; i < 16; i++) {
	spiro[i].x = path0[i].x;
	spiro[i].y = path0[i].y;
	spiro[i].ty = path0[i].ty;
	nextknot[i] = knot0[i];
    } else if ( c==1 ) for (i = 0; i < 6; i++) {
	spiro[i].x = path1[i].x;
	spiro[i].y = path1[i].y;
	spiro[i].ty = path1[i].ty;
	nextknot[i] = knot1[i];
    } else if ( c==2 ) for (i = 0; i < 4; i++) {
	spiro[i].x = path2[i].x;
	spiro[i].y = path2[i].y;
	spiro[i].ty = path2[i].ty;
	nextknot[i] = knot2[i];
    } else if ( c==3 ) for (i = 0; i < 6; i++) {
	spiro[i].x = path3[i].x;
	spiro[i].y = path3[i].y;
	spiro[i].ty = path3[i].ty;
	nextknot[i] = knot3[i];
    } else if ( c==4 ) for (i = 0; i < 4; i++) {
	spiro[i].x = path4[i].x;
	spiro[i].y = path4[i].y;
	spiro[i].ty = path4[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==5 ) for (i = 0; i < 12; i++) {
	spiro[i].x = path5[i].x;
	spiro[i].y = path5[i].y;
	spiro[i].ty = path5[i].ty;
	nextknot[i] = knot5[i];
    } else if ( c==6 ) for (i = 0; i < 12; i++) {
	spiro[i].x = path6[i].x;
	spiro[i].y = path6[i].y;
	spiro[i].ty = path6[i].ty;
	nextknot[i] = knot6[i];
    } else if ( c==7 || c==8 ) for (i = 0; i < 8; i++) {
	/* path7[]_co[7]==closed_curve, path8[]_co[8]==open_curve */
	spiro[i].x = path7[i].x;
	spiro[i].y = path7[i].y;
	spiro[i].ty = path7[i].ty;
	nextknot[i] = knot7[i];
    } else if ( c==9 ) for (i = 0; i < 4; i++) {
	/* path9[] is closed curve version of path4[] open curve. */
	spiro[i].x = path4[i].x;
	spiro[i].y = path4[i].y;
	spiro[i].ty = path4[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==10 || c==11 ) for (i = 0; i < 8; i++) {
	/* path10[]_co[10]=closedcurve, path11[]_co[10]=opencurve */
	spiro[i].x = path10[i].x;
	spiro[i].y = path10[i].y;
	spiro[i].ty = path10[i].ty;
	nextknot[i] = knot10[i];
    } else  if ( c==12 ) for (i = 0; i < 9; i++) {
	/* call_test12 checks curve ending in ah with following z */
	/* and declare cl[12] len=8 so run_spiro() can work okay. */
	spiro[i].x = path7[i].x;
	spiro[i].y = path7[i].y;
	spiro[i].ty = path7[i].ty;
	nextknot[i] = knot7[i];
    } else if ( c==13 ) for (i = 0; i < 8; i++) {
	/* path13[] is open curve based on path11[] using '{','}' */
	spiro[i].x = path13[i].x;
	spiro[i].y = path13[i].y;
	spiro[i].ty = path13[i].ty;
	nextknot[i] = knot13[i];
    } else if ( c==14 ) for (i = 0; i < 4; i++) {
	/* path14[] is a very big version of closed curve path9[] */
	spiro[i].x = path14[i].x;
	spiro[i].y = path14[i].y;
	spiro[i].ty = path14[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==15 ) for (i = 0; i < 4; i++) {
	/* path15[] is a very small copy of path9[] closed curve. */
	spiro[i].x = path15[i].x;
	spiro[i].y = path15[i].y;
	spiro[i].ty = path15[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==16 ) for (i = 0; i < 4; i++) {
	/* path16[] uses path4[] as first try at quadratic output */
	spiro[i].x = path4[i].x;
	spiro[i].y = path4[i].y;
	spiro[i].ty = path4[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==17 ) for (i = 0; i < 4; i++) {
	/* path17[] is closed curve version of path4[] open curve */
	spiro[i].x = path4[i].x;
	spiro[i].y = path4[i].y;
	spiro[i].ty = path4[i].ty;
	nextknot[i] = knot4[i];
    } else if ( c==18 ) for (i = 0; i < 4; i++) {
	spiro[i].x = path2[i].x;
	spiro[i].y = path2[i].y;
	spiro[i].ty = path2[i].ty;
	nextknot[i] = knot2[i];
    } else if ( c==19 ) for (i = 0; i < 16; i++) {
	spiro[i].x = path0[i].x;
	spiro[i].y = path0[i].y;
	spiro[i].ty = path0[i].ty;
	nextknot[i] = knot0[i];
    } else if ( c==20 || c==21 || c==22) for (i = 0; i < 9; i++) {
	/* path20[]=closed, path21[]=open, path22[]=closed_with_z */
	spiro[i].x = path7[i].x;
	spiro[i].y = path7[i].y;
	spiro[i].ty = path7[i].ty;
    } else if ( c==23 ) for (i = 0; i < 12; i++) {
	spiro[i].x = path5[i].x;
	spiro[i].y = path5[i].y;
	spiro[i].ty = path5[i].ty;
    } else if ( c==24 ) for (i = 0; i < 16; i++) {
	spiro[i].x = path0[i].x;
	spiro[i].y = path0[i].y;
	spiro[i].ty = path0[i].ty;
	nextknot[i] = knot0[i];
    }
}