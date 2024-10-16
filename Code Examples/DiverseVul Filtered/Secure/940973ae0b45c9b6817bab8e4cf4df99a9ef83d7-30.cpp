static bool ide_atapi_gesn_needed(void *opaque)
{
    IDEState *s = opaque;

    return s->events.new_media || s->events.eject_request;
}