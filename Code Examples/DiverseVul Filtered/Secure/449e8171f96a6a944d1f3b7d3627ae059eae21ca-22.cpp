static void drop_supplementary_groups(void)
{
    int ret;

    ret = getgroups(0, NULL);
    if (ret == -1) {
        fuse_log(FUSE_LOG_ERR, "getgroups() failed with error=%d:%s\n",
                 errno, strerror(errno));
        exit(1);
    }

    if (!ret) {
        return;
    }

    /* Drop all supplementary groups. We should not need it */
    ret = setgroups(0, NULL);
    if (ret == -1) {
        fuse_log(FUSE_LOG_ERR, "setgroups() failed with error=%d:%s\n",
                 errno, strerror(errno));
        exit(1);
    }
}