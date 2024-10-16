static inline void tftp_session_update(struct tftp_session *spt)
{
    spt->timestamp = curtime;
}