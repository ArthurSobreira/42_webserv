
#include "CGI.hpp"

// Default constructor
CGI::CGI() { return; }

// Copy constructor
CGI::CGI(const CGI &other) {
  *this = other;
  return;
}

// Copy assignment overload
CGI &CGI::operator=(const CGI &rhs) {
  (void)rhs;
  return *this;
}

// Default destructor
CGI::~CGI() { return; }
