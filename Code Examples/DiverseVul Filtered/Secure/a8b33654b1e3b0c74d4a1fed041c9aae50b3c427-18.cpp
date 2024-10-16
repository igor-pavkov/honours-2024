static void multi_timeout(unsigned long data)
{
	struct mp_port *mtpt = (struct mp_port *)data;


	spin_lock(&mtpt->port.lock);
	multi_handle_port(mtpt);
	spin_unlock(&mtpt->port.lock);

	mod_timer(&mtpt->timer, jiffies+1 );
}