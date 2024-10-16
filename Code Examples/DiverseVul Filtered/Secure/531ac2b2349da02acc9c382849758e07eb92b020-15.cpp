static int have_ask_password(void) {
        _cleanup_closedir_ DIR *dir;

        dir = opendir("/run/systemd/ask-password");
        if (!dir) {
                if (errno == ENOENT)
                        return false;
                else
                        return -errno;
        }

        for (;;) {
                struct dirent *de;

                errno = 0;
                de = readdir(dir);
                if (!de && errno > 0)
                        return -errno;
                if (!de)
                        return false;

                if (startswith(de->d_name, "ask."))
                        return true;
        }
}