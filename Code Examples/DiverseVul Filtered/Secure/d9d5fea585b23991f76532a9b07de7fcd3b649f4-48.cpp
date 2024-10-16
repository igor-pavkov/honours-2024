static void Log_dealloc(LogObject *self)
{
    if (self->s) {
        if (!self->expired)
            Log_call(self, self->s, self->l);

        free(self->s);
    }

    PyObject_Del(self);
}