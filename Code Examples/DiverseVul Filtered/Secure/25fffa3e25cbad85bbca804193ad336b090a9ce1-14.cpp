void ImapModelOpenConnectionTest::init()
{
    Common::registerMetaTypes();
    qRegisterMetaType<Imap::Mailbox::ImapTask*>();
    init( false );
}