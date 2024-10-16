PJ_DEF(pj_status_t) pjmedia_rtcp_enable_xr( pjmedia_rtcp_session *sess, 
					    pj_bool_t enable)
{
#if defined(PJMEDIA_HAS_RTCP_XR) && (PJMEDIA_HAS_RTCP_XR != 0)

    /* Check if request won't change anything */
    if (!(enable ^ sess->xr_enabled))
	return PJ_SUCCESS;

    if (!enable) {
	sess->xr_enabled = PJ_FALSE;
	return PJ_SUCCESS;
    }

    pjmedia_rtcp_xr_init(&sess->xr_session, sess, 0, 1);
    sess->xr_enabled = PJ_TRUE;

    return PJ_SUCCESS;

#else

    PJ_UNUSED_ARG(sess);
    PJ_UNUSED_ARG(enable);
    return PJ_ENOTSUP;

#endif
}