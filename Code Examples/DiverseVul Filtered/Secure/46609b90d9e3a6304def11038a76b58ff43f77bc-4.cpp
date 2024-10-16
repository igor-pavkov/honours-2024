static bool qtest_check_clang_sanitizer(void)
{
#ifdef QEMU_SANITIZE_ADDRESS
    return true;
#else
    g_test_skip("QEMU not configured using --enable-sanitizers");
    return false;
#endif
}