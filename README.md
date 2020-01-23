# AES

Simple implementation of Advanced Encryption Standards, with 128-, 192-, and 256-bit key lengths.
Usage:
3 mandatory arguments.  Arg1 is either  'e' for encrypt or 'd' for decrypt.  Arg2 is 16 byte message in hexadecimal (must be exactly 16 bytes, fewer should be 0-padded).  Arg3 is the key in hexadecimal.  Must be 16, 24, or 32 bytes. 
