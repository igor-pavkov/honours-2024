static int su_pam_conv(int num_msg, const struct pam_message **msg,
                       struct pam_response **resp, void *appdata_ptr)
{
	if (suppress_pam_info
	    && num_msg == 1
	    && msg
	    && msg[0]->msg_style == PAM_TEXT_INFO)
		return PAM_SUCCESS;
#ifdef HAVE_SECURITY_PAM_MISC_H
	return misc_conv(num_msg, msg, resp, appdata_ptr);
#elif defined(HAVE_SECURITY_OPENPAM_H)
	return openpam_ttyconv(num_msg, msg, resp, appdata_ptr);
#endif
}