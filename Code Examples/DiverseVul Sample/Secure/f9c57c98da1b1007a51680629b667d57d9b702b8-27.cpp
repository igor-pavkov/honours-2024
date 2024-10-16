void usage(po::options_description &desc) {
  cerr << "Usage: dnsreplay [OPTIONS] FILENAME [IP-ADDRESS] [PORT]"<<endl;
  cerr << desc << "\n";
}