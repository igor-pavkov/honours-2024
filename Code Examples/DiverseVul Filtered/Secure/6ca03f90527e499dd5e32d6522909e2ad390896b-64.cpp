int vt_get_shift_state(void)
{
        /* Don't lock as this is a transient report */
        return shift_state;
}