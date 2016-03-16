#ifndef _ARTDAQ_DATABASE_BASE64_CODEC_H_
#define _ARTDAQ_DATABASE_BASE64_CODEC_H_

#include <string>

// base64 encode / decode helper functions
std::string base64_encode(unsigned char const*, unsigned int);
std::string base64_encode(std::string const&);
std::string base64_decode(std::string const&);

#endif /* _ARTDAQ_DATABASE_BASE64_CODEC_H_ */
