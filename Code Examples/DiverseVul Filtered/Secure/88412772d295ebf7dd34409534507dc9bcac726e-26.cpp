char* XMLRPC_VALUE_ToXML(XMLRPC_VALUE val, int* buf_len) {
   xml_element *root_elem = XMLRPC_VALUE_to_xml_element(val);
   char* pRet = NULL;

   if(root_elem) {
      pRet = xml_elem_serialize_to_string(root_elem, NULL, buf_len);
      xml_elem_free(root_elem);
   }
   return pRet;
}