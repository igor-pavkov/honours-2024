static void test_cve_2021_20196(void)
{
    QTestState *s;

    if (!qtest_check_clang_sanitizer()) {
        return;
    }

    s = qtest_initf("-nographic -m 32M -nodefaults " DRIVE_FLOPPY_BLANK);

    qtest_outw(s, 0x3f4, 0x0500);
    qtest_outb(s, 0x3f5, 0x00);
    qtest_outb(s, 0x3f5, 0x00);
    qtest_outw(s, 0x3f4, 0x0000);
    qtest_outb(s, 0x3f5, 0x00);
    qtest_outw(s, 0x3f1, 0x0400);
    qtest_outw(s, 0x3f4, 0x0000);
    qtest_outw(s, 0x3f4, 0x0000);
    qtest_outb(s, 0x3f5, 0x00);
    qtest_outb(s, 0x3f5, 0x01);
    qtest_outw(s, 0x3f1, 0x0500);
    qtest_outb(s, 0x3f5, 0x00);
    qtest_quit(s);
}