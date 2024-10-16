int unit_coldplug(Unit *u) {
        int r = 0, q;
        char **i;

        assert(u);

        /* Make sure we don't enter a loop, when coldplugging recursively. */
        if (u->coldplugged)
                return 0;

        u->coldplugged = true;

        STRV_FOREACH(i, u->deserialized_refs) {
                q = bus_unit_track_add_name(u, *i);
                if (q < 0 && r >= 0)
                        r = q;
        }
        u->deserialized_refs = strv_free(u->deserialized_refs);

        if (UNIT_VTABLE(u)->coldplug) {
                q = UNIT_VTABLE(u)->coldplug(u);
                if (q < 0 && r >= 0)
                        r = q;
        }

        if (u->job) {
                q = job_coldplug(u->job);
                if (q < 0 && r >= 0)
                        r = q;
        }

        return r;
}