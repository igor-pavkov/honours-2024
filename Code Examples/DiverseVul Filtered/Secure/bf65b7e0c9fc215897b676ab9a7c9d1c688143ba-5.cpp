int unit_start(Unit *u) {
        UnitActiveState state;
        Unit *following;
        int r;

        assert(u);

        /* If this is already started, then this will succeed. Note that this will even succeed if this unit
         * is not startable by the user. This is relied on to detect when we need to wait for units and when
         * waiting is finished. */
        state = unit_active_state(u);
        if (UNIT_IS_ACTIVE_OR_RELOADING(state))
                return -EALREADY;

        /* Units that aren't loaded cannot be started */
        if (u->load_state != UNIT_LOADED)
                return -EINVAL;

        /* Refuse starting scope units more than once */
        if (UNIT_VTABLE(u)->once_only && dual_timestamp_is_set(&u->inactive_enter_timestamp))
                return -ESTALE;

        /* If the conditions failed, don't do anything at all. If we already are activating this call might
         * still be useful to speed up activation in case there is some hold-off time, but we don't want to
         * recheck the condition in that case. */
        if (state != UNIT_ACTIVATING &&
            !unit_test_condition(u)) {

                /* Let's also check the start limit here. Normally, the start limit is only checked by the
                 * .start() method of the unit type after it did some additional checks verifying everything
                 * is in order (so that those other checks can propagate errors properly). However, if a
                 * condition check doesn't hold we don't get that far but we should still ensure we are not
                 * called in a tight loop without a rate limit check enforced, hence do the check here. Note
                 * that ECOMM is generally not a reason for a job to fail, unlike most other errors here,
                 * hence the chance is big that any triggering unit for us will trigger us again. Note this
                 * condition check is a bit different from the condition check inside the per-unit .start()
                 * function, as this one will not change the unit's state in any way (and we shouldn't here,
                 * after all the condition failed). */

                r = unit_test_start_limit(u);
                if (r < 0)
                        return r;

                return log_unit_debug_errno(u, SYNTHETIC_ERRNO(ECOMM), "Starting requested but condition failed. Not starting unit.");
        }

        /* If the asserts failed, fail the entire job */
        if (state != UNIT_ACTIVATING &&
            !unit_test_assert(u))
                return log_unit_notice_errno(u, SYNTHETIC_ERRNO(EPROTO), "Starting requested but asserts failed.");

        /* Units of types that aren't supported cannot be started. Note that we do this test only after the
         * condition checks, so that we rather return condition check errors (which are usually not
         * considered a true failure) than "not supported" errors (which are considered a failure).
         */
        if (!unit_supported(u))
                return -EOPNOTSUPP;

        /* Let's make sure that the deps really are in order before we start this. Normally the job engine
         * should have taken care of this already, but let's check this here again. After all, our
         * dependencies might not be in effect anymore, due to a reload or due to a failed condition. */
        if (!unit_verify_deps(u))
                return -ENOLINK;

        /* Forward to the main object, if we aren't it. */
        following = unit_following(u);
        if (following) {
                log_unit_debug(u, "Redirecting start request from %s to %s.", u->id, following->id);
                return unit_start(following);
        }

        /* If it is stopped, but we cannot start it, then fail */
        if (!UNIT_VTABLE(u)->start)
                return -EBADR;

        /* We don't suppress calls to ->start() here when we are already starting, to allow this request to
         * be used as a "hurry up" call, for example when the unit is in some "auto restart" state where it
         * waits for a holdoff timer to elapse before it will start again. */

        unit_add_to_dbus_queue(u);

        return UNIT_VTABLE(u)->start(u);
}