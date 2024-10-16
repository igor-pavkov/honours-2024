XMLRPC_SERVER XMLRPC_GetGlobalServer() {
   static XMLRPC_SERVER xsServer = 0;
   if(!xsServer) {
      xsServer = XMLRPC_ServerCreate();
   }
   return xsServer;
}