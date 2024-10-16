static void Sleep(int millisecs)
{
    const struct timespec rqtp = { 0 , millisecs*1000000 } ;
    struct timespec   rmtp ;
    nanosleep(&rqtp,&rmtp);
}