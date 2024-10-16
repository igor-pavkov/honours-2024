static bool allow_object_pid(const struct ucred *ucred) {
        return ucred && ucred->uid == 0;
}