static void test_cve_2021_3507(void)
{
    QTestState *s;

    s = qtest_initf("-nographic -m 32M -nodefaults "
                    "-drive file=%s,format=raw,if=floppy,snapshot=on",
                    test_image);
    qtest_outl(s, 0x9, 0x0a0206);
    qtest_outw(s, 0x3f4, 0x1600);
    qtest_outw(s, 0x3f4, 0x0000);
    qtest_outw(s, 0x3f4, 0x0000);
    qtest_outw(s, 0x3f4, 0x0000);
    qtest_outw(s, 0x3f4, 0x0200);
    qtest_outw(s, 0x3f4, 0x0200);
    qtest_outw(s, 0x3f4, 0x0000);
    qtest_outw(s, 0x3f4, 0x0000);
    qtest_outw(s, 0x3f4, 0x0000);
    qtest_quit(s);
}