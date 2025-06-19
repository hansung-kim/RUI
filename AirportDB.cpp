//---------------------------------------------------------------------------

#pragma hdrstop

#include "AirportDB.h"
#include "csv.h"
 #include <cstring>
#include <string.h>

#define DIM(array)         (sizeof(array) / sizeof(array[0]))
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
ght_hash_table_t *AirportDBHashTable=NULL;
TList *airportList;
//---------------------------------------------------------------------------
static unsigned char mg_unhex_nimble(unsigned char c) {
  return (c >= '0' && c <= '9')   ? (unsigned char) (c - '0')
         : (c >= 'A' && c <= 'F') ? (unsigned char) (c - '7')
                                  : (unsigned char) (c - 'W');
}

//---------------------------------------------------------------------------
static unsigned long mg_unhexn(const char *s, size_t len) {
  unsigned long i = 0, v = 0;
  for (i = 0; i < len; i++) v <<= 4, v |= mg_unhex_nimble(((uint8_t *) s)[i]);
  return v;
}
//---------------------------------------------------------------------------
static void mg_unhex(const char *buf, size_t len, unsigned char *to) {
  size_t i;
  for (i = 0; i < len; i += 2) {
    to[i >> 1] = (unsigned char) mg_unhexn(&buf[i], 2);
  }
}

//---------------------------------------------------------------------------
 static int CSV_callback (struct CSV_context *ctx, const char *value)
{
  int    rc = 1;
  static bool  Init=true;
  static TAirportData Record;

  if (Init)
    {
      for (int i=0; i < AP_DB_NUM_FIELDS; i++)
      {
		 Record.Fields[i]="?";
         Init=false;
      }
    }
   if(strlen(value)) Record.Fields[ctx->field_num]=value;
   if (ctx->field_num==0)
    {
     std::strcpy((char*)Record.Code, value);
     //printf("%x %s\n",Record.Code,Record.Fields[ctx->field_num].c_str());
    }
   else if (ctx->field_num == (ctx->num_fields - 1))
   {
	 TAirportData *Data;
	 if (Record.Code!=0)
	 {
	  Data =(TAirportData *) ght_get(AirportDBHashTable,sizeof(Record.Code),&Record.Code);
	  if (Data)
	  {
	   printf("Duplicate Airport Record %s %x\n",Data->Fields[0].c_str(),Record.Code);
	  }
	  else if (std::strcmp((char*)Record.Fields[3].c_str(),"?") != 0) {
	   Data= new TAirportData;
       std::strcpy((char*)Data->Code, (char*)Record.Code);
	   for (int i=0; i < AP_DB_NUM_FIELDS; i++)  Data->Fields[i]=Record.Fields[i];
	   if (ght_insert(AirportDBHashTable,Data,sizeof(Data->Code), &Data->Code) < 0)
			 {
			  printf("ght_insert Error-Should Not Happen");
			 }
//        airportList->Add(Data);
	   // printf("Record Added %s\n",Data->Fields[0].c_str());
	  }
	 }
	 for (int i=0; i < AP_DB_NUM_FIELDS; i++) Record.Fields[i]="?";
   }
  return(rc);
}

bool InitAirportDB(AnsiString FileName)
{
  CSV_context  csv_ctx;
  AirportDBHashTable = ght_create(600000);
  ght_set_rehash(AirportDBHashTable, TRUE);

  if ( !AirportDBHashTable)
	{
	  throw Sysutils::Exception("Create Hash Failed");
	}
  ght_set_rehash(AirportDBHashTable, TRUE);
   memset (&csv_ctx, 0, sizeof(csv_ctx));
   csv_ctx.file_name = FileName.c_str();
   csv_ctx.delimiter = ',';
   csv_ctx.callback  = CSV_callback;
   csv_ctx.line_size = 2000;
   printf("Reading Airport DB\n");
   if (!CSV_open_and_parse_file(&csv_ctx))
    {
	  printf("Parsing of \"%s\" failed: %s\n", FileName.c_str(), strerror(errno));
      return (false);
	}

   printf("Done Reading Airport DB\n");
   return(true);
}
//---------------------------------------------------------------------------
bool GetAirportDBInfo(char *addr, double &latitude, double &longitude)
{
  static char          buf [2048];
  const TAirportData *a;
  unsigned char l_addr[5];
  std::strcpy((char*)l_addr, addr);
  a =(TAirportData *) ght_get(AirportDBHashTable,sizeof(l_addr),&l_addr);
  if (a)
   {
      latitude = StrToFloat(a->Fields[6]);
      longitude = StrToFloat(a->Fields[7]);
      return true;
   }
   return false;
}

ght_hash_table_t * getAirportDBHashTable() {
   return AirportDBHashTable;
};
//---------------------------------------------------------------------------

