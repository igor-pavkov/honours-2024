static void rtcp_init_seq(pjmedia_rtcp_session *sess)
{
    sess->received = 0;
    sess->exp_prior = 0;
    sess->rx_prior = 0;
    sess->transit = 0;
    sess->jitter = 0;
}