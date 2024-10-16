static int unit_add_dependency_hashmap(
                Hashmap **h,
                Unit *other,
                UnitDependencyMask origin_mask,
                UnitDependencyMask destination_mask) {

        UnitDependencyInfo info;
        int r;

        assert(h);
        assert(other);
        assert(origin_mask < _UNIT_DEPENDENCY_MASK_FULL);
        assert(destination_mask < _UNIT_DEPENDENCY_MASK_FULL);
        assert(origin_mask > 0 || destination_mask > 0);

        r = hashmap_ensure_allocated(h, NULL);
        if (r < 0)
                return r;

        assert_cc(sizeof(void*) == sizeof(info));

        info.data = hashmap_get(*h, other);
        if (info.data) {
                /* Entry already exists. Add in our mask. */

                if (FLAGS_SET(origin_mask, info.origin_mask) &&
                    FLAGS_SET(destination_mask, info.destination_mask))
                        return 0; /* NOP */

                info.origin_mask |= origin_mask;
                info.destination_mask |= destination_mask;

                r = hashmap_update(*h, other, info.data);
        } else {
                info = (UnitDependencyInfo) {
                        .origin_mask = origin_mask,
                        .destination_mask = destination_mask,
                };

                r = hashmap_put(*h, other, info.data);
        }
        if (r < 0)
                return r;

        return 1;
}