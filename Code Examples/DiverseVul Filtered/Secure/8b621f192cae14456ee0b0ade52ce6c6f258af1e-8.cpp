PJ_DEF(void) pjmedia_rtcp_init_stat(pjmedia_rtcp_stat *stat)
{
    pj_time_val now;

    pj_assert(stat);

    pj_bzero(stat, sizeof(pjmedia_rtcp_stat));

    pj_math_stat_init(&stat->rtt);
    pj_math_stat_init(&stat->rx.loss_period);
    pj_math_stat_init(&stat->rx.jitter);
    pj_math_stat_init(&stat->tx.loss_period);
    pj_math_stat_init(&stat->tx.jitter);

#if defined(PJMEDIA_RTCP_STAT_HAS_IPDV) && PJMEDIA_RTCP_STAT_HAS_IPDV!=0
    pj_math_stat_init(&stat->rx_ipdv);
#endif

#if defined(PJMEDIA_RTCP_STAT_HAS_RAW_JITTER) && PJMEDIA_RTCP_STAT_HAS_RAW_JITTER!=0
    pj_math_stat_init(&stat->rx_raw_jitter);
#endif

    pj_gettimeofday(&now);
    stat->start = now;
}