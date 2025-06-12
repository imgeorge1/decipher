#include <emscripten.h>
#include <string>
#include <cstring> // for strcpy
#include <cstdlib> // for malloc, free

int binaryToDecimal(const std::string &binary)
{
  int result = 0;
  for (char bit : binary)
  {
    if (bit == '0' || bit == '1')
      result = result * 2 + (bit - '0');
    else
      return -1; // invalid character
  }
  return result;
}

extern "C" EMSCRIPTEN_KEEPALIVE char *decode(const char *input)
{
  std::string output;
  std::string byte;

  // Shift character forward by 3, with wrap-around
  auto shiftChar = [](char c) -> char
  {
    if (c >= 'A' && c <= 'Z')
      return (c - 'A' + 3) % 26 + 'A'; // add 3, wrap around A-Z
    else if (c >= 'a' && c <= 'z')
      return (c - 'a' + 3) % 26 + 'a'; // add 3, wrap around a-z
    else
      return c; // keep punctuation, spaces, etc.
  };

  for (const char *p = input; *p; ++p)
  {
    if (*p == ' ')
    {
      if (!byte.empty())
      {
        int value = binaryToDecimal(byte);
        if (value >= 0)
        {
          char c = static_cast<char>(value);
          output += shiftChar(c);
        }
        byte.clear();
      }
    }
    else
    {
      if (*p == '0' || *p == '1')
        byte += *p;
      else
        ; // ignore invalid chars
    }
  }

  // Handle last byte
  if (!byte.empty())
  {
    int value = binaryToDecimal(byte);
    if (value >= 0)
      output += shiftChar(static_cast<char>(value));
  }

  // Allocate memory for output string
  size_t len = output.size();
  char *buffer = (char *)malloc(len + 1);
  memcpy(buffer, output.c_str(), len);
  buffer[len] = '\0';

  return buffer;
}
