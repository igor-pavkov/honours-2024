char * getStringTimeFromSec(double seconds)
{
    int hour[3];
    char * ret;
    char * HourTime;
    char * MinTime;

    if (seconds <0)
        return NULL;

    ret = (char *) calloc(1,256);

    HourTime = (char *) calloc (1,128);
    MinTime  = (char *) calloc (1,128);

    hour[0]  = (int) (seconds);
    hour[1]  = hour[0] / 60;
    hour[2]  = hour[1] / 60;
    hour[0] %= 60 ;
    hour[1] %= 60 ;

    if (hour[2] != 0 )
        snprintf(HourTime, 128, "%d %s", hour[2], ( hour[2] == 1 ) ? "hour" : "hours");
    if (hour[1] != 0 )
        snprintf(MinTime, 128, "%d %s", hour[1], ( hour[1] == 1 ) ? "min" : "mins");

    if ( hour[2] != 0 && hour[1] != 0 )
        snprintf(ret, 256, "%s %s", HourTime, MinTime);
    else
    {
        if (hour[2] == 0 && hour[1] == 0)
            snprintf(ret, 256, "%d s", hour[0] );
        else
            snprintf(ret, 256, "%s", (hour[2] == 0) ? MinTime : HourTime );
    }

    free(MinTime);
    free(HourTime);

    return ret;

}