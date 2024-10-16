static void *vmstate_base_addr(void *opaque, VMStateField *field)
{
    void *base_addr = opaque + field->offset;

    if (field->flags & VMS_POINTER) {
        base_addr = *(void **)base_addr + field->start;
    }

    return base_addr;
}