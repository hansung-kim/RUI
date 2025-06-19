//---------------------------------------------------------------------------

#pragma hdrstop

#include "RouteDB.h"
#include "csv.h"
 #include <cstring>
#include <string.h>

#define DIM(array)         (sizeof(array) / sizeof(array[0]))
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
ght_hash_table_t *RouteDBHashTable=NULL;
//---------------------------------------------------------------------------
 static int CSV_callback (struct CSV_context *ctx, const char *value)
{
  int    rc = 1;
  static bool  Init=true;
  static TRouteData Record;

  if (Init)
    {
      for (int i=0; i < ROUTE_DB_NUM_FIELDS; i++)
      {
		 Record.Fields[i]="?";
         Init=false;
      }
    }
   if(strlen(value)) Record.Fields[ctx->field_num]=value;
   if (ctx->field_num==0)
    {
      std::strncpy((char*)Record.Callsign, value, 8);
    }
   else if (ctx->field_num == (ctx->num_fields - 1))
   {
	 TRouteData *Data;
	 if (Record.Callsign!=0)
	 {
	  Data =(TRouteData *) ght_get(RouteDBHashTable,sizeof(Record.Callsign),&Record.Callsign);
	  if (Data)
	  {
	   //printf("Duplicate Airport Record %s %x\n",Data->Fields[0].c_str(),Record.Code);
	  }
	  else {
	   Data= new TRouteData;
       std::strncpy((char*)Data->Callsign, (char*)Record.Callsign, 8);
	   for (int i=0; i < ROUTE_DB_NUM_FIELDS; i++)  Data->Fields[i]=Record.Fields[i];
	   if (ght_insert(RouteDBHashTable,Data,sizeof(Data->Callsign), &Data->Callsign) < 0)
			 {
			  printf("ght_insert Error-Should Not Happen");
			 }
//        airportList->Add(Data);
	   // printf("Record Added %s\n",Data->Fields[0].c_str());
	  }
	 }
	 for (int i=0; i < ROUTE_DB_NUM_FIELDS; i++) Record.Fields[i]="?";
   }
  return(rc);
}

bool InitRouteDB(AnsiString FileName)
{
  CSV_context  csv_ctx;
  RouteDBHashTable = ght_create(600000);
  ght_set_rehash(RouteDBHashTable, TRUE);

  if ( !RouteDBHashTable)
	{
	  throw Sysutils::Exception("Create Hash Failed");
	}
   ght_set_rehash(RouteDBHashTable, TRUE);
   memset (&csv_ctx, 0, sizeof(csv_ctx));
   csv_ctx.file_name = FileName.c_str();
   csv_ctx.delimiter = ',';
   csv_ctx.callback  = CSV_callback;
   csv_ctx.line_size = 2000;
   printf("Reading Route DB\n");
   if (!CSV_open_and_parse_file(&csv_ctx))
    {
	  printf("Parsing of \"%s\" failed: %s\n", FileName.c_str(), strerror(errno));
      return (false);
	}

   printf("Done Reading Route DB\n");
   return(true);
}
//---------------------------------------------------------------------------

const char * GetRouteDBInfo(char *addr, bool &bFind)
{
  static char          buf [2048];
  const TRouteData *a;
  unsigned char l_addr[8];
  std::strncpy((char*)l_addr, addr, 8);
  a =(TRouteData *) ght_get(RouteDBHashTable,sizeof(l_addr),&l_addr);

  if (a)
   {
      snprintf (buf,sizeof(buf),"%s",a->Fields[4].c_str());
      bFind = true;
   }
   else {
      snprintf (buf,sizeof(buf),"addr: 0x%06X, No Data",addr);
      bFind = false;
   }

   return (buf);
}

ght_hash_table_t * getRouteDBHashTable() {
   return RouteDBHashTable;
};
//---------------------------------------------------------------------------

