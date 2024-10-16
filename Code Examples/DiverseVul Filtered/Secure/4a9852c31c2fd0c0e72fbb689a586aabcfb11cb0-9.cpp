static void set_tac_srv_key(unsigned int srv_no, const char *key) {
    if (srv_no < TAC_PLUS_MAXSERVERS) {
        if (key) {
            strncpy(tac_srv_key[srv_no], key, TAC_SECRET_MAX_LEN - 1);
            tac_srv[srv_no].key = tac_srv_key[srv_no];
        }
        else {
            _pam_log(LOG_DEBUG, "%s: server %d key is null; address [%s]", __FUNCTION__,srv_no,
                              tac_ntop(tac_srv[srv_no].addr->ai_addr));
            tac_srv[srv_no].key = NULL;
        }
    }
}