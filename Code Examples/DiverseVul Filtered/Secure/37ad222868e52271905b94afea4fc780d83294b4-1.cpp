void donoop(void)
{
#ifdef BORING_MODE
    addreply_noformat(200, "dc.w $4E71");
#else
    addreply_noformat(200, MSG_SLEEPING);
#endif
}