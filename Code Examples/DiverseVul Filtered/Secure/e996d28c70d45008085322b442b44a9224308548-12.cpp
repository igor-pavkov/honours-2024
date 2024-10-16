static void timestamp(void)
{
    struct compile_time * const oct = &official_compile_time;
    const struct tm *tp, *best_gm;

    time(&oct->t);

    best_gm = NULL;

    tp = localtime(&oct->t);
    if (tp) {
        oct->local = *tp;
        best_gm = &oct->local;
        oct->have_local = true;
    }

    tp = gmtime(&oct->t);
    if (tp) {
        oct->gm = *tp;
        best_gm = &oct->gm;
        oct->have_gm = true;
        if (!oct->have_local)
            oct->local = oct->gm;
    } else {
        oct->gm = oct->local;
    }

    if (best_gm) {
        oct->posix = make_posix_time(best_gm);
        oct->have_posix = true;
    }
}