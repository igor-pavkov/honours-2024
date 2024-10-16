static void sigchild(int sig)
{
    const int olderrno = errno;
    pid_t pid;

    (void) sig;
# ifdef HAVE_WAITPID
    while ((pid = waitpid((pid_t) -1, NULL, WNOHANG)) > (pid_t) 0) {
        if (nb_children > 0U) {
            nb_children--;
        }
#  ifdef FTPWHO
        ftpwho_unlinksbfile(pid);
#  endif
        iptrack_delete_pid(pid);
    }
# endif
    errno = olderrno;
}