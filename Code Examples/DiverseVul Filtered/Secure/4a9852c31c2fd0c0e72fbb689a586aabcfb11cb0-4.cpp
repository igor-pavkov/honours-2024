int tacacs_get_password(pam_handle_t *pamh, int flags __Unused,
                        int ctrl, char **password) {

    (void) flags;
    const void *pam_pass;
    char *pass = NULL;

    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: called", __FUNCTION__);

    if ((ctrl & (PAM_TAC_TRY_FIRST_PASS | PAM_TAC_USE_FIRST_PASS))
        && (pam_get_item(pamh, PAM_AUTHTOK, &pam_pass) == PAM_SUCCESS)
        && (pam_pass != NULL)) {
        if ((pass = strdup(pam_pass)) == NULL)
            return PAM_BUF_ERR;
    } else if ((ctrl & PAM_TAC_USE_FIRST_PASS)) {
        _pam_log(LOG_WARNING, "no forwarded password");
        return PAM_PERM_DENIED;
    } else {
        struct pam_message msg;
        struct pam_response *resp = NULL;
        int retval;

        /* set up conversation call */
        msg.msg_style = PAM_PROMPT_ECHO_OFF;

        if (!tac_prompt[0]) {
            msg.msg = "Password: ";
        } else {
            msg.msg = tac_prompt;
        }

        if ((retval = converse(pamh, 1, &msg, &resp)) != PAM_SUCCESS)
            return retval;

        if (resp != NULL) {
            if (resp->resp == NULL && (ctrl & PAM_TAC_DEBUG))
                _pam_log(LOG_DEBUG, "pam_sm_authenticate: NULL authtok given");

            pass = resp->resp;    /* remember this! */
            resp->resp = NULL;

            free(resp);
            resp = NULL;
        } else {
            if (ctrl & PAM_TAC_DEBUG) {
                _pam_log(LOG_DEBUG, "pam_sm_authenticate: no error reported");
                _pam_log(LOG_DEBUG, "getting password, but NULL returned!?");
            }
            return PAM_CONV_ERR;
        }
    }

    /*
       FIXME *password can still turn out as NULL
       and it can't be free()d when it's NULL
    */
    *password = pass;       /* this *MUST* be free()'d by this module */

    if (ctrl & PAM_TAC_DEBUG)
        syslog(LOG_DEBUG, "%s: obtained password", __FUNCTION__);

    return PAM_SUCCESS;
}