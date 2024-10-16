static int local_set_xattrat(int dirfd, const char *path, FsCred *credp)
{
    int err;

    if (credp->fc_uid != -1) {
        uint32_t tmp_uid = cpu_to_le32(credp->fc_uid);
        err = fsetxattrat_nofollow(dirfd, path, "user.virtfs.uid", &tmp_uid,
                                   sizeof(uid_t), 0);
        if (err) {
            return err;
        }
    }
    if (credp->fc_gid != -1) {
        uint32_t tmp_gid = cpu_to_le32(credp->fc_gid);
        err = fsetxattrat_nofollow(dirfd, path, "user.virtfs.gid", &tmp_gid,
                                   sizeof(gid_t), 0);
        if (err) {
            return err;
        }
    }
    if (credp->fc_mode != -1) {
        uint32_t tmp_mode = cpu_to_le32(credp->fc_mode);
        err = fsetxattrat_nofollow(dirfd, path, "user.virtfs.mode", &tmp_mode,
                                   sizeof(mode_t), 0);
        if (err) {
            return err;
        }
    }
    if (credp->fc_rdev != -1) {
        uint64_t tmp_rdev = cpu_to_le64(credp->fc_rdev);
        err = fsetxattrat_nofollow(dirfd, path, "user.virtfs.rdev", &tmp_rdev,
                                   sizeof(dev_t), 0);
        if (err) {
            return err;
        }
    }
    return 0;
}