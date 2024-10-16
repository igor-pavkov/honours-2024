static void oss_anal_close (int *fdp)
{
    int err;

    qemu_set_fd_handler (*fdp, NULL, NULL, NULL);
    err = close (*fdp);
    if (err) {
        oss_logerr (errno, "Failed to close file(fd=%d)\n", *fdp);
    }
    *fdp = -1;
}