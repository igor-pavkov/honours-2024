static void lo_restore_cred_gain_cap(struct lo_cred *old, bool restore_umask,
                                     const char *cap_name)
{
    assert(cap_name);

    lo_restore_cred(old, restore_umask);

    if (gain_effective_cap(cap_name)) {
        fuse_log(FUSE_LOG_ERR, "Failed to gain CAP_%s\n", cap_name);
    }
}