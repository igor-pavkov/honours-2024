static void lo_getlk(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi,
                     struct flock *lock)
{
    struct lo_data *lo = lo_data(req);
    struct lo_inode *inode;
    struct lo_inode_plock *plock;
    int ret, saverr = 0;

    fuse_log(FUSE_LOG_DEBUG,
             "lo_getlk(ino=%" PRIu64 ", flags=%d)"
             " owner=0x%" PRIx64 ", l_type=%d l_start=0x%" PRIx64
             " l_len=0x%" PRIx64 "\n",
             ino, fi->flags, fi->lock_owner, lock->l_type,
             (uint64_t)lock->l_start, (uint64_t)lock->l_len);

    if (!lo->posix_lock) {
        fuse_reply_err(req, ENOSYS);
        return;
    }

    inode = lo_inode(req, ino);
    if (!inode) {
        fuse_reply_err(req, EBADF);
        return;
    }

    pthread_mutex_lock(&inode->plock_mutex);
    plock =
        lookup_create_plock_ctx(lo, inode, fi->lock_owner, lock->l_pid, &ret);
    if (!plock) {
        saverr = ret;
        goto out;
    }

    ret = fcntl(plock->fd, F_OFD_GETLK, lock);
    if (ret == -1) {
        saverr = errno;
    }

out:
    pthread_mutex_unlock(&inode->plock_mutex);
    lo_inode_put(lo, &inode);

    if (saverr) {
        fuse_reply_err(req, saverr);
    } else {
        fuse_reply_lock(req, lock);
    }
}