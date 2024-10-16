void doretr(char *name)
{
    DLHandler dlhandler;
    int f;
    struct stat st;
    double started = 0.0;
    int ret;

    dlhandler.total_downloaded = 0;
    if (!candownload) {
        addreply(550, MSG_LOAD_TOO_HIGH, load);
        goto end;
    }
    if (type < 1 || (type == 1 && restartat > (off_t) 1)) {
        addreply_noformat(503, MSG_NO_ASCII_RESUME);
        goto end;
    }
    if (checknamesanity(name, dot_read_ok) != 0) {
        addreply(553, MSG_SANITY_FILE_FAILURE, name);
        goto end;
    }
    if ((f = open(name, O_RDONLY)) == -1) {
        displayopenfailure(name);
        goto end;
    }
    if (fstat(f, &st) < 0) {
        stat_failure:
        (void) close(f);
        error(451, MSG_STAT_FAILURE);
        goto end;
    }
    if (S_ISLNK(st.st_mode)) {
        if (stat(name, &st) < 0) {
            goto stat_failure;
        }
    }
    if (restartat > st.st_size) {
        (void) close(f);
        addreply(554, MSG_REST_TOO_LARGE_FOR_FILE "\n" MSG_REST_RESET,
                 (long long) restartat, (long long) st.st_size);
        goto end;
    }
    if (!S_ISREG(st.st_mode) || ((off_t) st.st_size != st.st_size)) {
        (void) close(f);
        addreply_noformat(550, MSG_NOT_REGULAR_FILE);
        goto end;
    }
    if (warez != 0 && st.st_uid == warez && guest != 0) {
        (void) close(f);
        addreply(550, MSG_NOT_MODERATED);
        goto end;
    }
#ifdef RATIOS
    if (ratio_upload > 0U && ratio_download > 0U) {
        if ((downloaded + st.st_size - restartat) / ratio_download >
            (uploaded / ratio_upload)) {
            (void) close(f);
            addreply(550, MSG_RATIO_DENIAL, ratio_upload, ratio_download,
                     (unsigned long long) uploaded / 1024ULL,
                     (unsigned long long) downloaded / 1024ULL);
            goto end;
        }
    }
#endif
    opendata();
    if (xferfd == -1) {
        (void) close(f);
        goto end;
    }
#ifndef DISABLE_HUMOR
    if ((time(NULL) % 100) == 0) {
        addreply_noformat(0, MSG_WINNER);
    }
#endif
    if (st.st_size - restartat > 4096) {
        addreply(0, MSG_KBYTES_LEFT, (double) ((st.st_size - restartat) / 1024.0));
    }
    doreply();
# ifdef WITH_TLS
    if (data_protection_level == CPL_PRIVATE) {
        tls_init_data_session(xferfd, passive);
    }
# endif
    state_needs_update = 1;
    setprocessname("pure-ftpd (DOWNLOAD)");

#ifdef FTPWHO
    if (shm_data_cur != NULL) {
        const size_t sl = strlen(name);

        ftpwho_lock();
        shm_data_cur->state = FTPWHO_STATE_DOWNLOAD;
        shm_data_cur->download_total_size = st.st_size;
        shm_data_cur->download_current_size = restartat;
        shm_data_cur->restartat = restartat;
        (void) time(&shm_data_cur->xfer_date);
        if (sl < sizeof shm_data_cur->filename) {
            memcpy(shm_data_cur->filename, name, sl);
            shm_data_cur->filename[sl] = 0;
        } else {
            memcpy(shm_data_cur->filename,
                   &name[sl - sizeof shm_data_cur->filename - 1U],
                   sizeof shm_data_cur->filename);
        }
        ftpwho_unlock();
    }
#endif
#ifdef HAVE_POSIX_FADVISE
    (void) posix_fadvise(f, (off_t) 0, st.st_size, POSIX_FADV_SEQUENTIAL);
#endif

    started = get_usec_time();

    if (dlmap_init(&dlhandler, clientfd, tls_cnx, xferfd, name, f,
                   tls_data_cnx, restartat, type == 1,
                   throttling_bandwidth_dl) == 0) {
        ret = dlmap_send(&dlhandler);
        dlmap_exit(&dlhandler);
    } else {
        ret = -1;
    }

    (void) close(f);
    closedata();
    if (ret == 0) {
        addreply_noformat(226, MSG_TRANSFER_SUCCESSFUL);
    }
    downloaded += dlhandler.total_downloaded;
    displayrate(MSG_DOWNLOADED, dlhandler.total_downloaded, started, name, 0);

    end:
    restartat = (off_t) 0;
}