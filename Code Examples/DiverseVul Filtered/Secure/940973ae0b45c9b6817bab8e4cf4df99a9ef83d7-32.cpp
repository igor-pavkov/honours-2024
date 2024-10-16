static bool is_identify_set(void *opaque, int version_id)
{
    IDEState *s = opaque;

    return s->identify_set != 0;
}