int unit_setup_dynamic_creds(Unit *u) {
        ExecContext *ec;
        DynamicCreds *dcreds;
        size_t offset;

        assert(u);

        offset = UNIT_VTABLE(u)->dynamic_creds_offset;
        assert(offset > 0);
        dcreds = (DynamicCreds*) ((uint8_t*) u + offset);

        ec = unit_get_exec_context(u);
        assert(ec);

        if (!ec->dynamic_user)
                return 0;

        return dynamic_creds_acquire(dcreds, u->manager, ec->user, ec->group);
}