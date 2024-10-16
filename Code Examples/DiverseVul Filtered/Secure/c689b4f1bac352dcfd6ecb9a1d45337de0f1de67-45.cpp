static GuestFileHandle *guest_file_handle_find(int64_t id, Error **err)
{
    GuestFileHandle *gfh;

    QTAILQ_FOREACH(gfh, &guest_file_state.filehandles, next)
    {
        if (gfh->id == id) {
            return gfh;
        }
    }

    error_setg(err, "handle '%" PRId64 "' has not been found", id);
    return NULL;
}