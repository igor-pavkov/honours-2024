static inline int tftp_session_in_use(struct tftp_session *spt)
{
    return (spt->slirp != NULL);
}