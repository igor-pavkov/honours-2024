void qmp_guest_file_flush(int64_t handle, Error **err)
{
    GuestFileHandle *gfh = guest_file_handle_find(handle, err);
    FILE *fh;
    int ret;

    if (!gfh) {
        return;
    }

    fh = gfh->fh;
    ret = fflush(fh);
    if (ret == EOF) {
        error_setg_errno(err, errno, "failed to flush file");
    }
}