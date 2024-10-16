static void lo_copy_file_range(fuse_req_t req, fuse_ino_t ino_in, off_t off_in,
                               struct fuse_file_info *fi_in, fuse_ino_t ino_out,
                               off_t off_out, struct fuse_file_info *fi_out,
                               size_t len, int flags)
{
    int in_fd, out_fd;
    ssize_t res;

    in_fd = lo_fi_fd(req, fi_in);
    out_fd = lo_fi_fd(req, fi_out);

    fuse_log(FUSE_LOG_DEBUG,
             "lo_copy_file_range(ino=%" PRIu64 "/fd=%d, "
             "off=%ju, ino=%" PRIu64 "/fd=%d, "
             "off=%ju, size=%zd, flags=0x%x)\n",
             ino_in, in_fd, (intmax_t)off_in,
             ino_out, out_fd, (intmax_t)off_out, len, flags);

    res = copy_file_range(in_fd, &off_in, out_fd, &off_out, len, flags);
    if (res < 0) {
        fuse_reply_err(req, errno);
    } else {
        fuse_reply_write(req, res);
    }
}