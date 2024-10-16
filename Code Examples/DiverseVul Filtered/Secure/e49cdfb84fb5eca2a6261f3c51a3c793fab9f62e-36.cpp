static void ok_inflater_set_error(ok_inflater *inflater) {
    if (inflater) {
        inflater->state = OK_INFLATER_STATE_ERROR;
    }
}