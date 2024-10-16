static bool check_uid_of_pid(pid_t pid, uid_t uid)
{
    char fn[128];
    struct stat st;

    snprintf(fn, sizeof(fn), "/proc/%u/status", (unsigned) pid);
    if (stat(fn, &st) != 0 || st.st_uid != uid) {
        return false;
    }
    return true;
}