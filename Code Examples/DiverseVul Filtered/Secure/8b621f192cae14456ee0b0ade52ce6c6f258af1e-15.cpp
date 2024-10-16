PJ_DEF(void) pjmedia_rtcp_session_setting_default(
				    pjmedia_rtcp_session_setting *settings)
{
    pj_bzero(settings, sizeof(*settings));
}