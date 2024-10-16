void sighandler( int signum)
{
	ssize_t unused;
    int card=0;

    signal( signum, sighandler );

    if( signum == SIGUSR1 )
    {
		unused = read( G.cd_pipe[0], &card, sizeof(int) );
        if(G.freqoption)
            unused = read( G.ch_pipe[0], &(G.frequency[card]), sizeof( int ) );
        else
            unused = read( G.ch_pipe[0], &(G.channel[card]), sizeof( int ) );
    }

    if( signum == SIGUSR2 )
        unused = read( G.gc_pipe[0], &G.gps_loc, sizeof( float ) * 5 );

    if( signum == SIGINT || signum == SIGTERM )
    {
	reset_term();
        alarm( 1 );
        G.do_exit = 1;
        signal( SIGALRM, sighandler );
        dprintf( STDOUT_FILENO, "\n" );
    }

    if( signum == SIGSEGV )
    {
        fprintf( stderr, "Caught signal 11 (SIGSEGV). Please"
                         " contact the author!\33[?25h\n\n" );
        fflush( stdout );
        exit( 1 );
    }

    if( signum == SIGALRM )
    {
        dprintf( STDERR_FILENO, "Caught signal 14 (SIGALRM). Please"
                         " contact the author!\33[?25h\n\n" );
        _exit( 1 );
    }

    if( signum == SIGCHLD )
        wait( NULL );

    if( signum == SIGWINCH )
    {
        fprintf( stderr, "\33[2J" );
        fflush( stdout );
    }
}