void qmp_guest_file_close(int64_t handle, Error **err)
{
    GuestFileHandle *gfh = guest_file_handle_find(handle, err);
    int ret;

    slog("guest-file-close called, handle: %ld", handle);
    if (!gfh) {
        return;
    }

    ret = fclose(gfh->fh);
    if (ret == EOF) {
        error_setg_errno(err, errno, "failed to close handle");
        return;
    }

    QTAILQ_REMOVE(&guest_file_state.filehandles, gfh, next);
    g_free(gfh);
}