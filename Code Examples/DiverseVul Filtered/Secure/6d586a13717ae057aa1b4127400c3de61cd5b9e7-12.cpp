static int append_iovec(sd_bus_message *m, const void *p, size_t sz) {
        assert(m);
        assert(p);
        assert(sz > 0);

        m->iovec[m->n_iovec++] = IOVEC_MAKE((void*) p, sz);

        return 0;
}