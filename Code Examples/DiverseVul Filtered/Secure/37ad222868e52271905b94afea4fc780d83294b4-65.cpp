static int fortune(void)
{
    int fd;
    char *buf;
    char *bufpnt;
    char *bufend;
    struct stat st;
    off_t gl;
    char *fortunepnt;
    char fortune[2048];

    if (fortunes_file == NULL || *fortunes_file == 0) {
        return 0;
    }
    if ((fd = open(fortunes_file, O_RDONLY)) == -1) {
        logfile(LOG_ERR, MSG_OPEN_FAILURE, fortunes_file);
        return -1;
    }
    if (fstat(fd, &st) < 0 ||
        (((S_IRUSR | S_IRGRP | S_IROTH) & st.st_mode) !=
         (S_IRUSR | S_IRGRP | S_IROTH)) ||
        !(S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)) || st.st_size < 2 ||
        (buf = mmap(NULL, (size_t) st.st_size,
                PROT_READ, MAP_FILE | MAP_SHARED, fd,
                (off_t) 0)) == (void *) MAP_FAILED) {
        (void) close(fd);
        logfile(LOG_ERR, MSG_OPEN_FAILURE, fortunes_file);
        return -1;
    }
# ifdef HAVE_RANDOM
    gl = (off_t) (random() % (st.st_size - 1U));
# else
    gl = (off_t) (rand() % (st.st_size - 1U));
# endif
    bufpnt = buf + gl;
    bufend = buf + st.st_size;
    while (bufpnt != buf) {
        if (bufpnt[0] == '\n') {
            if (&bufpnt[-1] != buf && bufpnt[-1] == '%') {
                if (&bufpnt[-2] != buf && bufpnt[-2] == '\n') {
                    break;
                }
            }
        }
        bufpnt--;
    }
    if (bufpnt != buf) {
        while (bufpnt != bufend && *bufpnt == '\n') {
            bufpnt++;
        }
    }
    fortunepnt = fortune;
    while (*bufpnt != 0 && bufpnt != bufend &&
           fortunepnt != &fortune[sizeof fortune - 1U]) {
        if (bufpnt[0] == '\n') {
            if (&bufpnt[1] != bufend && bufpnt[1] == '%') {
                if (&bufpnt[2] != bufend && bufpnt[2] == '\n') {
                    break;
                }
            }
        }
        *fortunepnt++ = *bufpnt++;
    }
    if (fortunepnt == fortune) {
        goto bye;
    }
    do {
        fortunepnt--;
    } while (fortunepnt != fortune && (*fortunepnt == '\n' ||
                                       isspace((unsigned char) *fortunepnt)));
    fortunepnt[1] = 0;
    fortunepnt = fortune;
    while (*fortunepnt == '\n') {
        fortunepnt++;
    }
    if (*fortunepnt == 0) {
        goto bye;
    }
    addreply(220, "%s", fortunepnt);
    bye:
    (void) munmap(buf, st.st_size);
    (void) close(fd);

    return 1;
}