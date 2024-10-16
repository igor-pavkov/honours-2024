static int bus_message_setup_iovec(sd_bus_message *m) {
        struct bus_body_part *part;
        unsigned n, i;
        int r;

        assert(m);
        assert(m->sealed);

        if (m->n_iovec > 0)
                return 0;

        assert(!m->iovec);

        n = 1 + m->n_body_parts;
        if (n < ELEMENTSOF(m->iovec_fixed))
                m->iovec = m->iovec_fixed;
        else {
                m->iovec = new(struct iovec, n);
                if (!m->iovec) {
                        r = -ENOMEM;
                        goto fail;
                }
        }

        r = append_iovec(m, m->header, BUS_MESSAGE_BODY_BEGIN(m));
        if (r < 0)
                goto fail;

        MESSAGE_FOREACH_PART(part, i, m)  {
                r = bus_body_part_map(part);
                if (r < 0)
                        goto fail;

                r = append_iovec(m, part->data, part->size);
                if (r < 0)
                        goto fail;
        }

        assert(n == m->n_iovec);

        return 0;

fail:
        m->poisoned = true;
        return r;
}