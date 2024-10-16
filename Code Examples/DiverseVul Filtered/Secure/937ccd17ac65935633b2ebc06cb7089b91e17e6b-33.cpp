ENUM_PTRS_WITH(gx_ttfReader_enum_ptrs, gx_ttfReader *mptr)
    {
        /* The fields 'pfont' and 'glyph_data' may contain pointers from global
           to local memory ( see a comment in gxttfb.h).
           They must be NULL when a garbager is invoked.
           Due to that we don't enumerate and don't relocate them.
         */
        DISCARD(mptr);
        return 0;
    }