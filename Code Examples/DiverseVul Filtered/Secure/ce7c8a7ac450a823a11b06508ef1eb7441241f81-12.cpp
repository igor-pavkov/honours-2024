off_t _q_iosend(FILE *outfp, FILE *infp, off_t nbytes) {
    if (nbytes == 0) return 0;

    unsigned char buf[QIOSEND_CHUNK_SIZE];
    off_t total = 0; // total size sent
    while (total < nbytes) {
        size_t chunksize; // this time sending size
        if (nbytes - total <= sizeof(buf)) chunksize = nbytes - total;
        else chunksize = sizeof(buf);

        // read
        size_t rsize = fread(buf, 1, chunksize, infp);
        if (rsize == 0) break;
        DEBUG("read %zu", rsize);

        // write
        size_t wsize = fwrite(buf, 1, rsize, outfp);
        if (wsize == 0) break;
        DEBUG("write %zu", wsize);

        total += wsize;
        if (rsize != wsize) {
            DEBUG("size mismatch. read:%zu, write:%zu", rsize, wsize);
            break;
        }
    }

    if (total > 0) return total;
    return -1;
}