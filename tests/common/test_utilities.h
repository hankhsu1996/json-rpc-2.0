#pragma once

#include <iostream>
#include <sstream>

// Helper function to redirect stdin
inline void setInput(const std::string &input) {
  std::istringstream *in = new std::istringstream(input);
  std::cin.rdbuf(in->rdbuf());
}

// Helper function to capture stdout
inline std::string getOutput() {
  std::ostringstream out;
  std::cout.rdbuf(out.rdbuf());
  return out.str();
}

// Structure to ensure proper redirection and restoration of std::cin and
// std::cout
struct RedirectIO {
  RedirectIO(std::istream &in, std::ostream &out)
      : oldCinBuf(std::cin.rdbuf()), oldCoutBuf(std::cout.rdbuf()) {
    std::cin.rdbuf(in.rdbuf());
    std::cout.rdbuf(out.rdbuf());
  }

  ~RedirectIO() {
    std::cin.rdbuf(oldCinBuf);
    std::cout.rdbuf(oldCoutBuf);
  }

  std::streambuf *oldCinBuf;
  std::streambuf *oldCoutBuf;
};
