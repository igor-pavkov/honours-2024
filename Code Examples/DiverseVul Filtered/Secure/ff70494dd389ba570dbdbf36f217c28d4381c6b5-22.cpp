void frequency_hopper(struct wif *wi[], int if_num, int chan_count )
{
	ssize_t unused;
    int ch, ch_idx = 0, card=0, chi=0, cai=0, j=0, k=0, first=1, again=1;
    int dropped=0;

    while( getppid() != 1 )
    {
        for( j = 0; j < if_num; j++ )
        {
            again = 1;

            ch_idx = chi % chan_count;

            card = cai % if_num;

            ++chi;
            ++cai;

            if( G.chswitch == 2 && !first )
            {
                j = if_num - 1;
                card = if_num - 1;

                if( getfreqcount(1) > if_num )
                {
                    while( again )
                    {
                        again = 0;
                        for( k = 0; k < ( if_num - 1 ); k++ )
                        {
                            if( G.own_frequencies[ch_idx] == G.frequency[k] )
                            {
                                again = 1;
                                ch_idx = chi % chan_count;
                                chi++;
                            }
                        }
                    }
                }
            }

            if( G.own_frequencies[ch_idx] == -1 )
            {
                j--;
                cai--;
                dropped++;
                if(dropped >= chan_count)
                {
                    ch = wi_get_freq(wi[card]);
                    G.frequency[card] = ch;
                    unused = write( G.cd_pipe[1], &card, sizeof(int) );
                    unused = write( G.ch_pipe[1], &ch, sizeof( int ) );
                    kill( getppid(), SIGUSR1 );
                    usleep(1000);
                }
                continue;
            }

            dropped = 0;

            ch = G.own_frequencies[ch_idx];

            if(wi_set_freq(wi[card], ch ) == 0 )
            {
                G.frequency[card] = ch;
                unused = write( G.cd_pipe[1], &card, sizeof(int) );
                unused = write( G.ch_pipe[1], &ch, sizeof( int ) );
                kill( getppid(), SIGUSR1 );
                usleep(1000);
            }
            else
            {
                G.own_frequencies[ch_idx] = -1;      /* remove invalid channel */
                j--;
                cai--;
                continue;
            }
        }

        if(G.chswitch == 0)
        {
            chi=chi-(if_num - 1);
        }

        if(first)
        {
            first = 0;
        }

        usleep( (G.hopfreq*1000) );
    }

    exit( 0 );
}