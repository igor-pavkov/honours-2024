void ImapModelOpenConnectionTest::testInitialGarbage_data()
{
    QTest::addColumn<QByteArray>("greetings");

    QTest::newRow("utter-garbage")
        << QByteArray("blesmrt trojita\r\n");

    QTest::newRow("pop3")
        << QByteArray("+OK InterMail POP3 server ready.\r\n");
}