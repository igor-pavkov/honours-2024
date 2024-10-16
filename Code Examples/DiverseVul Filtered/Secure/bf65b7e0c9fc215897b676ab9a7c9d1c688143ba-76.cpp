int unit_set_invocation_id(Unit *u, sd_id128_t id) {
        int r;

        assert(u);

        /* Set the invocation ID for this unit. If we cannot, this will not roll back, but reset the whole thing. */

        if (sd_id128_equal(u->invocation_id, id))
                return 0;

        if (!sd_id128_is_null(u->invocation_id))
                (void) hashmap_remove_value(u->manager->units_by_invocation_id, &u->invocation_id, u);

        if (sd_id128_is_null(id)) {
                r = 0;
                goto reset;
        }

        r = hashmap_ensure_allocated(&u->manager->units_by_invocation_id, &id128_hash_ops);
        if (r < 0)
                goto reset;

        u->invocation_id = id;
        sd_id128_to_string(id, u->invocation_id_string);

        r = hashmap_put(u->manager->units_by_invocation_id, &u->invocation_id, u);
        if (r < 0)
                goto reset;

        return 0;

reset:
        u->invocation_id = SD_ID128_NULL;
        u->invocation_id_string[0] = 0;
        return r;
}