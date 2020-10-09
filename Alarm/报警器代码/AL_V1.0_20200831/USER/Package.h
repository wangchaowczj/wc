#ifndef _PACKAGE_H_
#define _PACKAGE_H_

extern void PackageKeyInit(void);
extern ErrorStatus Unpack(STR_COMM* data_in, STR_COMM* data_out);
extern void Pack(STR_COMM* data_in, STR_COMM* data_out);

#endif
