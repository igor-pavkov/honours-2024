static int log_unit_internal(void *userdata, int level, int error, const char *file, int line, const char *func, const char *format, ...) {
        Unit *u = userdata;
        va_list ap;
        int r;

        va_start(ap, format);
        if (u)
                r = log_object_internalv(level, error, file, line, func,
                                         u->manager->unit_log_field,
                                         u->id,
                                         u->manager->invocation_log_field,
                                         u->invocation_id_string,
                                         format, ap);
        else
                r = log_internalv(level, error,  file, line, func, format, ap);
        va_end(ap);

        return r;
}