static bool is_safe_path_component(const char *path)
{
    if (strchr(path, '/')) {
        return false;
    }

    return !is_dot_or_dotdot(path);
}