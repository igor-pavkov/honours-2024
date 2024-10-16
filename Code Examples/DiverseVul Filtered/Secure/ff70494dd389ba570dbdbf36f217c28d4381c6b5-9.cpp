int getfrequencies(const char *optarg)
{
    unsigned int i=0,freq_cur=0,freq_first=0,freq_last=0,freq_max=10000,freq_remain=0;
    char *optfreq = NULL, *optc;
    char *token = NULL;
    int *tmp_frequencies;

    //got a NULL pointer?
    if(optarg == NULL)
        return -1;

    freq_remain=freq_max;

    //create a writable string
    optc = optfreq = (char*) malloc(strlen(optarg)+1);
    strncpy(optfreq, optarg, strlen(optarg));
    optfreq[strlen(optarg)]='\0';

    tmp_frequencies = (int*) malloc(sizeof(int)*(freq_max+1));

    //split string in tokens, separated by ','
    while( (token = strsep(&optfreq,",")) != NULL)
    {
        //range defined?
        if(strchr(token, '-') != NULL)
        {
            //only 1 '-' ?
            if(strchr(token, '-') == strrchr(token, '-'))
            {
                //are there any illegal characters?
                for(i=0; i<strlen(token); i++)
                {
                    if( (token[i] < '0' || token[i] > '9') && (token[i] != '-'))
                    {
                        free(tmp_frequencies);
                        free(optc);
                        return -1;
                    }
                }

                if( sscanf(token, "%d-%d", &freq_first, &freq_last) != EOF )
                {
                    if(freq_first > freq_last)
                    {
                        free(tmp_frequencies);
                        free(optc);
                        return -1;
                    }
                    for(i=freq_first; i<=freq_last; i++)
                    {
                        if( (! invalid_frequency(i)) && (freq_remain > 0) )
                        {
                                tmp_frequencies[freq_max-freq_remain]=i;
                                freq_remain--;
                        }
                    }
                }
                else
                {
                    free(tmp_frequencies);
                    free(optc);
                    return -1;
                }

            }
            else
            {
                free(tmp_frequencies);
                free(optc);
                return -1;
            }
        }
        else
        {
            //are there any illegal characters?
            for(i=0; i<strlen(token); i++)
            {
                if( (token[i] < '0') && (token[i] > '9') )
                {
                    free(tmp_frequencies);
                    free(optc);
                    return -1;
                }
            }

            if( sscanf(token, "%d", &freq_cur) != EOF)
            {
                if( (! invalid_frequency(freq_cur)) && (freq_remain > 0) )
                {
                        tmp_frequencies[freq_max-freq_remain]=freq_cur;
                        freq_remain--;
                }

                /* special case "-C 0" means: scan all available frequencies */
                if(freq_cur == 0)
                {
                    freq_first = 1;
                    freq_last = 9999;
                    for(i=freq_first; i<=freq_last; i++)
                    {
                        if( (! invalid_frequency(i)) && (freq_remain > 0) )
                        {
                                tmp_frequencies[freq_max-freq_remain]=i;
                                freq_remain--;
                        }
                    }
                }

            }
            else
            {
                free(tmp_frequencies);
                free(optc);
                return -1;
            }
        }
    }

    G.own_frequencies = (int*) malloc(sizeof(int)*(freq_max - freq_remain + 1));

    for(i=0; i<(freq_max - freq_remain); i++)
    {
        G.own_frequencies[i]=tmp_frequencies[i];
    }

    G.own_frequencies[i]=0;

    free(tmp_frequencies);
    free(optc);
    if(i==1) return G.own_frequencies[0];   //exactly 1 frequency given
    if(i==0) return -1;                     //error occured
    return 0;                               //frequency hopping
}