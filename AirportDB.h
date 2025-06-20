//---------------------------------------------------------------------------

#ifndef AirportDBH
#define AirportDBH
#include "ght_hash_table.h"

#define  AP_DB_NUM_FIELDS           9
#define  AP_DB_CODE                 0
#define  AP_DB_NAME                 1
#define  AP_DB_ICAO                 2
#define  AP_DB_IATA                 3
#define  AP_DB_LOCATION             4
#define  AP_DB_COUNTRY_ISO2         5
#define  AP_DB_LATITUDE             6
#define  AP_DB_LONGITUDE            7
#define  AP_DB_ALTITUDE_FEET        8

typedef struct
{
  unsigned char Code[5];
  AnsiString  Fields[AP_DB_NUM_FIELDS];
} TAirportData;

bool InitAirportDB(AnsiString FileName);
TAirportData* GetAirportDBInfo(char *addr, double &latitude, double &longitude);
ght_hash_table_t * getAirportDBHashTable();
//---------------------------------------------------------------------------
#endif
