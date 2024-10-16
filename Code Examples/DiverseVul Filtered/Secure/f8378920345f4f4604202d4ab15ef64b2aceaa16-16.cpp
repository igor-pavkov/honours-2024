void Subgraph::SetName(const char* name) {
  if (name) {
    name_ = name;
  } else {
    name_ = "";
  }
}