JWT::JWT(
    const Header& _header,
    const JSON::Object& _payload,
    const Option<string>& _signature)
  : header(_header), payload(_payload), signature(_signature) {}