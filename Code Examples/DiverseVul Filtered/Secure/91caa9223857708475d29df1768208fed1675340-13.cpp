static void udscs_connection_finalize(GObject *obj)
{
    UdscsConnection *self = UDSCS_CONNECTION(obj);

    if (self->debug) {
        syslog(LOG_DEBUG, "%p disconnected", self);
    }

    G_OBJECT_CLASS(udscs_connection_parent_class)->finalize(obj);
}