static bool warning_is_error(int severity)
{
    if (is_valid_warning(severity))
        return !!(warning_state[WARN_IDX(severity)] & WARN_ST_ERROR);
    else
        return false;
}