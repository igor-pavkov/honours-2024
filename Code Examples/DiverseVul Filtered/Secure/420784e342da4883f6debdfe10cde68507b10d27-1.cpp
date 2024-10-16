_kc_signal_to_string(int sig)
{
    switch (sig) {
    case 0:
        return "no signal (0)";
    case SIGKILL:
        return "SIGKILL (" G_STRINGIFY(SIGKILL) ")";
    case SIGTERM:
        return "SIGTERM (" G_STRINGIFY(SIGTERM) ")";
    default:
        return "Unexpected signal";
    }
}