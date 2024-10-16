int converse(pam_handle_t *pamh, int nargs, const struct pam_message *message,
             struct pam_response **response) {

    int retval;
    struct pam_conv *conv;

    if ((retval = pam_get_item(pamh, PAM_CONV, (const void **) &conv)) == PAM_SUCCESS) {
        retval = conv->conv(nargs, &message, response, conv->appdata_ptr);

        if (retval != PAM_SUCCESS) {
            _pam_log(LOG_ERR, "(pam_tacplus) converse returned %d", retval);
            _pam_log(LOG_ERR, "that is: %s", pam_strerror(pamh, retval));
        }
    } else {
        _pam_log(LOG_ERR, "(pam_tacplus) converse failed to get pam_conv");
    }

    return retval;
}