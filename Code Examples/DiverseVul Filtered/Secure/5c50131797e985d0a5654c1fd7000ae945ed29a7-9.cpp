uid_t session_info_uid_for_session(struct session_info *si, const char *session)
{
    uid_t ret = -1;
    if (sd_session_get_uid(session, &ret) < 0) {
        return -1;
    }
    return ret;
}