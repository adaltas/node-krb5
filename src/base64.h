#ifndef BASE64_H_INCLUDED
#define BASE64_H_INCLUDED

void encode64 (char *orig, char *dest, int nbcar);
int decode64 (char *buffer);

#endif // BASE64_H_INCLUDED
