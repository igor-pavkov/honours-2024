static void define_macros_early(void)
{
    const struct compile_time * const oct = &official_compile_time;
    char temp[128];

    if (oct->have_local) {
        strftime(temp, sizeof temp, "__DATE__=\"%Y-%m-%d\"", &oct->local);
        preproc->pre_define(temp);
        strftime(temp, sizeof temp, "__DATE_NUM__=%Y%m%d", &oct->local);
        preproc->pre_define(temp);
        strftime(temp, sizeof temp, "__TIME__=\"%H:%M:%S\"", &oct->local);
        preproc->pre_define(temp);
        strftime(temp, sizeof temp, "__TIME_NUM__=%H%M%S", &oct->local);
        preproc->pre_define(temp);
    }

    if (oct->have_gm) {
        strftime(temp, sizeof temp, "__UTC_DATE__=\"%Y-%m-%d\"", &oct->gm);
        preproc->pre_define(temp);
        strftime(temp, sizeof temp, "__UTC_DATE_NUM__=%Y%m%d", &oct->gm);
        preproc->pre_define(temp);
        strftime(temp, sizeof temp, "__UTC_TIME__=\"%H:%M:%S\"", &oct->gm);
        preproc->pre_define(temp);
        strftime(temp, sizeof temp, "__UTC_TIME_NUM__=%H%M%S", &oct->gm);
        preproc->pre_define(temp);
    }

    if (oct->have_posix) {
        snprintf(temp, sizeof temp, "__POSIX_TIME__=%"PRId64, oct->posix);
        preproc->pre_define(temp);
    }
}