GetDefaultConnectionService(struct upnphttp * h, const char * action, const char * ns)
{
	static const char resp[] =
		"<u:%sResponse "
		"xmlns:u=\"%s\">"
#ifdef IGD_V2
		"<NewDefaultConnectionService>%s:WANConnectionDevice:2,"
#else
		"<NewDefaultConnectionService>%s:WANConnectionDevice:1,"
#endif
		SERVICE_ID_WANIPC "</NewDefaultConnectionService>"
		"</u:%sResponse>";
	/* example from UPnP_IGD_Layer3Forwarding 1.0.pdf :
	 * uuid:44f5824f-c57d-418c-a131-f22b34e14111:WANConnectionDevice:1,
	 * urn:upnp-org:serviceId:WANPPPConn1 */
	char body[1024];
	int bodylen;

	/* namespace : urn:schemas-upnp-org:service:Layer3Forwarding:1 */
	bodylen = snprintf(body, sizeof(body), resp,
	                   action, ns, uuidvalue_wcd, action);
	BuildSendAndCloseSoapResp(h, body, bodylen);
}