#ifndef __CAPABILITIES_H__
#define __CAPABILITIES_H__

typedef struct {
  char *representation;
  char *uuid;
  char *adminurl;
  char *duplex;
  char *is;
  char *cs;
  char *pdl;
  char *ty;
  char *vers;
} ippScanner;

int is_scanner_present(ippScanner *scanner, const char *name);
ippScanner *free_scanner(ippScanner *scanner);

#endif
