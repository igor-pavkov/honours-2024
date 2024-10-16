int session_info_get_fd(struct session_info *si)
{
    return sd_login_monitor_get_fd(si->mon);
}