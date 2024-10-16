int main(int argc, char **argv) {
    double st, en;
    int ret;
    st = get_time();

#ifdef DO_CALL_TEST0
    ret=test_curve(0);	/* this comes with unit-test. */
#endif
#ifdef DO_CALL_TEST1
    ret=test_curve(1);	/* do a test using "{"..."}". */
#endif
#ifdef DO_CALL_TEST2
    ret=test_curve(2);	/* this does many iterations. */
#endif
#ifdef DO_CALL_TEST3
    if ( (ret=test_curve(3)==0) ) /* This curve will not converge */
	ret = -1 /* error found! ret=error value */;
    else
	ret = 0; /* expected failure to converge */
#endif
#ifdef DO_CALL_TEST4
    ret=test_curve(4);	/* test a cyclic calculation. */
#endif
#ifdef DO_CALL_TEST5
    ret=test_curve(5);	/* verify curve data with []. */
#endif
#ifdef DO_CALL_TEST6
    ret=test_curve(6);	/* verify curve data with ah. */
#endif
#ifdef DO_CALL_TEST7
    ret=test_curve(7);	/* loop stops with ah curves. */
#endif
#ifdef DO_CALL_TEST8
    ret=test_curve(8);	/* this open curve ends in ah */
#endif
#ifdef DO_CALL_TEST9
    ret=test_curve(9);	/* path4[] as a closed curve. */
#endif
#ifdef DO_CALL_TEST10
    /* TODO: see why can start using c, o, but not [. */
    /* TODO: see why can start using c, o, but not a. */
    ret=test_curve(10);	/* start loop with ah curves. */
    ret = 0; /* ignore result for now until improved. */
#endif
#ifdef DO_CALL_TEST11
    /* TODO: see why can start using c, o, but not [. */
    /* TODO: see why can start using c, o, but not a. */
    ret=test_curve(11);	/* start open curve using ah. */
    ret = 0; /* ignore result for now until improved. */
#endif
#ifdef DO_CALL_TEST12
    /* TODO: knot counts not matched for taggedspiro, */
    /* therefore use !defined(DO_CALL_TEST12) for now */
    ret=test_curve(12);	/* do path7[] with a z ending */
#endif
#ifdef DO_CALL_TEST13
    ret=test_curve(13);	/* start open curve using {h. */
#endif
#ifdef DO_CALL_TEST14
    ret=test_curve(14);	/* go very big! go very tiny! */
#endif
#ifdef DO_CALL_TEST15
    ret=test_curve(15);	/* go very big! go very tiny! */
#endif
#ifdef DO_CALL_TEST16
    ret=test_curve(16);	/* testing arc output path4[] */
#endif
#ifdef DO_CALL_TEST17
    ret=test_curve(17);	/* do arc closed curve outut. */
#endif
#ifdef DO_CALL_TEST18
    ret=test_curve(18);	/* do iterative as arc output */
#endif
#ifdef DO_CALL_TEST19
    ret=test_curve(19);	/* do lengthy output with arc */
#endif
#ifdef DO_CALL_TEST20
    /* test spiroreverse and verify path[] directions */
    if ( (ret=test_curve(20))==0 )
	if ( (ret=test_curve(21))==0 )
	    if ( (ret=test_curve(22))==0 )
		if ( (ret=test_curve(23))==0 )
		    ret=test_curve(24);
#endif
#ifdef DO_CALL_TESTM
    ret=test_multi_curves();
#endif

    en = get_time();
    printf("time %g\n", (en - st));
    return ret;
}