char * getBatteryString(void)
{
    int batt_time;
    char * ret;
    char * batt_string;

    batt_time = getBatteryState();

    if ( batt_time <= 60 ) {
        ret = (char *) calloc(1,2);
        ret[0] = ']';
        return ret;
    }

    batt_string = getStringTimeFromSec( (double) batt_time );

    ret = (char *) calloc( 1, 256 );

    snprintf( ret, 256, "][ BAT: %s ]", batt_string );

    free( batt_string);

    return ret;
}