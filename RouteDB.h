//---------------------------------------------------------------------------

#ifndef RouteDBH
#define RouteDBH
#include "ght_hash_table.h"

#define  ROUTE_DB_NUM_FIELDS           5
#define  ROUTE_DB_CALLSIGN             0
#define  ROUTE_DB_CODE                 1
#define  ROUTE_DB_NUMBER               2
#define  ROUTE_DB_AIRLINE_CODE         3
#define  ROUTE_DB_AIRPORT_CODES        4


typedef struct
{
  unsigned char Callsign[8];
  AnsiString  Fields[ROUTE_DB_NUM_FIELDS];
} TRouteData;

bool InitRouteDB(AnsiString FileName);
const char * GetRouteDBInfo(char *addr, bool &bFind);
ght_hash_table_t * getRouteDBHashTable();
//---------------------------------------------------------------------------
#endif
