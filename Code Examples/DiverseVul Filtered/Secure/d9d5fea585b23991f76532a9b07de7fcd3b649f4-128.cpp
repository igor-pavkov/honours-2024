static void Input_dealloc(InputObject *self)
{
    if (self->buffer)
        free(self->buffer);

    PyObject_Del(self);
}