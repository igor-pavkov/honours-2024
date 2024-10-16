PJ_DEF(void) pjmedia_rtcp_fini(pjmedia_rtcp_session *sess)
{
#if defined(PJMEDIA_HAS_RTCP_XR) && (PJMEDIA_HAS_RTCP_XR != 0)
    pjmedia_rtcp_xr_fini(&sess->xr_session);
#else
    /* Nothing to do. */
    PJ_UNUSED_ARG(sess);
#endif
}