static void lo_init(void *userdata, struct fuse_conn_info *conn)
{
    struct lo_data *lo = (struct lo_data *)userdata;

    if (conn->capable & FUSE_CAP_EXPORT_SUPPORT) {
        conn->want |= FUSE_CAP_EXPORT_SUPPORT;
    }

    if (lo->writeback && conn->capable & FUSE_CAP_WRITEBACK_CACHE) {
        fuse_log(FUSE_LOG_DEBUG, "lo_init: activating writeback\n");
        conn->want |= FUSE_CAP_WRITEBACK_CACHE;
    }
    if (conn->capable & FUSE_CAP_FLOCK_LOCKS) {
        if (lo->flock) {
            fuse_log(FUSE_LOG_DEBUG, "lo_init: activating flock locks\n");
            conn->want |= FUSE_CAP_FLOCK_LOCKS;
        } else {
            fuse_log(FUSE_LOG_DEBUG, "lo_init: disabling flock locks\n");
            conn->want &= ~FUSE_CAP_FLOCK_LOCKS;
        }
    }

    if (conn->capable & FUSE_CAP_POSIX_LOCKS) {
        if (lo->posix_lock) {
            fuse_log(FUSE_LOG_DEBUG, "lo_init: activating posix locks\n");
            conn->want |= FUSE_CAP_POSIX_LOCKS;
        } else {
            fuse_log(FUSE_LOG_DEBUG, "lo_init: disabling posix locks\n");
            conn->want &= ~FUSE_CAP_POSIX_LOCKS;
        }
    }

    if ((lo->cache == CACHE_NONE && !lo->readdirplus_set) ||
        lo->readdirplus_clear) {
        fuse_log(FUSE_LOG_DEBUG, "lo_init: disabling readdirplus\n");
        conn->want &= ~FUSE_CAP_READDIRPLUS;
    }

    if (!(conn->capable & FUSE_CAP_SUBMOUNTS) && lo->announce_submounts) {
        fuse_log(FUSE_LOG_WARNING, "lo_init: Cannot announce submounts, client "
                 "does not support it\n");
        lo->announce_submounts = false;
    }

    if (lo->user_killpriv_v2 == 1) {
        /*
         * User explicitly asked for this option. Enable it unconditionally.
         * If connection does not have this capability, it should fail
         * in fuse_lowlevel.c
         */
        fuse_log(FUSE_LOG_DEBUG, "lo_init: enabling killpriv_v2\n");
        conn->want |= FUSE_CAP_HANDLE_KILLPRIV_V2;
        lo->killpriv_v2 = 1;
    } else if (lo->user_killpriv_v2 == -1 &&
               conn->capable & FUSE_CAP_HANDLE_KILLPRIV_V2) {
        /*
         * User did not specify a value for killpriv_v2. By default enable it
         * if connection offers this capability
         */
        fuse_log(FUSE_LOG_DEBUG, "lo_init: enabling killpriv_v2\n");
        conn->want |= FUSE_CAP_HANDLE_KILLPRIV_V2;
        lo->killpriv_v2 = 1;
    } else {
        /*
         * Either user specified to disable killpriv_v2, or connection does
         * not offer this capability. Disable killpriv_v2 in both the cases
         */
        fuse_log(FUSE_LOG_DEBUG, "lo_init: disabling killpriv_v2\n");
        conn->want &= ~FUSE_CAP_HANDLE_KILLPRIV_V2;
        lo->killpriv_v2 = 0;
    }

    if (lo->user_posix_acl == 1) {
        /*
         * User explicitly asked for this option. Enable it unconditionally.
         * If connection does not have this capability, print error message
         * now. It will fail later in fuse_lowlevel.c
         */
        if (!(conn->capable & FUSE_CAP_POSIX_ACL) ||
            !(conn->capable & FUSE_CAP_DONT_MASK) ||
            !(conn->capable & FUSE_CAP_SETXATTR_EXT)) {
            fuse_log(FUSE_LOG_ERR, "lo_init: Can not enable posix acl."
                     " kernel does not support FUSE_POSIX_ACL, FUSE_DONT_MASK"
                     " or FUSE_SETXATTR_EXT capability.\n");
        } else {
            fuse_log(FUSE_LOG_DEBUG, "lo_init: enabling posix acl\n");
        }

        conn->want |= FUSE_CAP_POSIX_ACL | FUSE_CAP_DONT_MASK |
                      FUSE_CAP_SETXATTR_EXT;
        lo->change_umask = true;
        lo->posix_acl = true;
    } else {
        /* User either did not specify anything or wants it disabled */
        fuse_log(FUSE_LOG_DEBUG, "lo_init: disabling posix_acl\n");
        conn->want &= ~FUSE_CAP_POSIX_ACL;
    }
}