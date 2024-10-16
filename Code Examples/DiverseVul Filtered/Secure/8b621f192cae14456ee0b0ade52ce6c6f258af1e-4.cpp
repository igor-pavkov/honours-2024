PJ_DEF(void) pjmedia_rtcp_init(pjmedia_rtcp_session *sess, 
			       char *name,
			       unsigned clock_rate,
			       unsigned samples_per_frame,
			       pj_uint32_t ssrc)
{
    pjmedia_rtcp_session_setting settings;

    pjmedia_rtcp_session_setting_default(&settings);
    settings.name = name;
    settings.clock_rate = clock_rate;
    settings.samples_per_frame = samples_per_frame;
    settings.ssrc = ssrc;

    pjmedia_rtcp_init2(sess, &settings);
}