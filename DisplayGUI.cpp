//---------------------------------------------------------------------------

#include <vcl.h>
#include <new>
#include <math.h>
#include <dir.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <filesystem>
#include <fileapi.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#pragma hdrstop

#include "DisplayGUI.h"
#include "AreaDialog.h"
#include "ntds2d.h"
#include "LatLonConv.h"
#include "PointInPolygon.h"
#include "DecodeRawADS_B.h"
#include "ght_hash_table.h"
#include "dms.h"
#include "Aircraft.h"
#include "TimeFunctions.h"
#include "SBS_Message.h"
#include "CPA.h"
#include "AircraftDB.h"
#include "csv.h"
#define USE_WORKER_THREAD
#include <IdHTTP.hpp>
#include <IdSSLOpenSSL.hpp>
#include <fstream>
#include "AirportDB.h"
#include "RouteDB.h"
#ifndef YAKI_TEST_CODE // check internet
#include <WinInet.h>
#include <System.Net.HttpClient.hpp>   // TNetHTTPClient
#include <System.Net.URLClient.hpp>    // CustomHeaders�� TNetHeaders ��
#endif

#define AIRCRAFT_DATABASE_URL   "https://opensky-network.org/datasets/metadata/aircraftDatabase.zip"
#define AIRCRAFT_DATABASE_FILE   "aircraftDatabase.csv"
#define ARTCC_BOUNDARY_FILE      "Ground_Level_ARTCC_Boundary_Data_2025-05-15.csv"

#define AIRPORT_DATABASE_URL   "https://vrs-standing-data.adsb.lol/airports.csv"
#define AIRPORT_DATABASE_FILE   "airportDatabase.csv"
#define ROUTE_DATABASE_URL   "https://vrs-standing-data.adsb.lol/routes.csv"
#define ROUTE_DATABASE_FILE   "routeDatabase.csv"

#define MAP_CENTER_LAT  40.73612;
#define MAP_CENTER_LON -80.33158;

#define BIG_QUERY_UPLOAD_COUNT 50000
#define BIG_QUERY_RUN_FILENAME  "SimpleCSVtoBigQuery.py"
#define   LEFT_MOUSE_DOWN   1
#define   RIGHT_MOUSE_DOWN  2
#define   MIDDLE_MOUSE_DOWN 4


#define BG_INTENSITY   0.37
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OpenGLPanel"
#pragma link "Map\libgefetch\Win64\Debug\libgefetch.a"
#pragma link "Map\zlib\Win64\Release\zlib.a"
#pragma link "Map\jpeg\Win64\Release\jpeg.a"
#pragma link "Map\png\Win64\Release\png.a"
#pragma link "HashTable\Lib\Win64\Release\HashTableLib.a"
#pragma link "cspin"
#pragma resource "*.dfm"
#ifndef YAKI_TEST_CODE // check internet
//#pragma comment(lib, "Wininet.lib")
#include <System.RegularExpressions.hpp>
#include <Vcl.Edge.hpp>
#include <vector>
#include "Unit2.h"
#endif
#ifndef YAKI_TEST_CODE
#include "MapFactory.h"
#include "GoogleMapProvider.h"
#include "SkyVectorMapProvider.h"
#include "OpenStreetMapProvider.h"
#endif

int log_time_cnt = 1;
int log_time_cnt_cpa = 1;
int log_time_cnt_raw = 1;
int log_time_cnt_sbs = 1;
int log_time_cnt_polygon = 1;
std::chrono::high_resolution_clock::time_point g_start_time_raw_connect;
std::chrono::high_resolution_clock::time_point g_start_time_sbs_connect;
std::chrono::high_resolution_clock::time_point g_start_time;
std::chrono::high_resolution_clock::time_point g_start_time_cpa;
std::chrono::high_resolution_clock::time_point g_start_time_polygon;

TForm1 *Form1;
 //---------------------------------------------------------------------------
 static void RunPythonScript(AnsiString scriptPath,AnsiString args);
 static bool DeleteFilesWithExtension(AnsiString dirPath, AnsiString extension);
 static int FinshARTCCBoundary(void);
 //---------------------------------------------------------------------------

static char *stristr(const char *String, const char *Pattern);
static const char * strnistr(const char * pszSource, DWORD dwLength, const char * pszFind) ;

//---------------------------------------------------------------------------
uint32_t createRGB(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}
//---------------------------------------------------------------------------
uint32_t PopularColors[] = {
	  createRGB(255, 0, 0),      // Red
	  createRGB(0, 255, 0),      // Green
	  createRGB(0, 0, 255),      // Blue
	  createRGB(255, 255, 0),   // Yellow
	  createRGB(255, 165, 0),   // Orange
	  createRGB(255, 192, 203), // Pink
	  createRGB(0, 255, 255),   // Cyan
	  createRGB(255, 0, 255),  // Magenta
	  createRGB(255,255, 255),    // White
	  //createRGB(0, 0, 0),        // Black
	  createRGB(128,128,128),      // Gray
	  createRGB(165,42,42)    // Brown
  };

  int NumColors = sizeof(PopularColors) / sizeof(PopularColors[0]);
 unsigned int CurrentColor=0;


 //---------------------------------------------------------------------------
 typedef struct
{
   union{ 
     struct{ 
	 System::Byte Red;
	 System::Byte Green;
	 System::Byte Blue;
	 System::Byte Alpha;
     }; 
     struct{ 
     TColor Cl; 
     }; 
     struct{ 
     COLORREF Rgb; 
     }; 
   };

}TMultiColor;


//-----------------------for BigQuery

void RunBigQueryScript(const std::string& start_time, const std::string& end_time) {
	printf("RunBigQueryScript!");
	printf("start_time: %s\n\n",start_time.c_str());
	printf("end_time: %s\n\n",end_time.c_str());
	AnsiString  BigQueryPythonPath = ExtractFilePath(ExtractFileDir(Application->ExeName))+"..\\BigQuery\\SimpleBigQueryToSBS.py";
	AnsiString  BigQueryPythonLogPath = ExtractFilePath(ExtractFileDir(Application->ExeName))+"..\\BigQuery\\bigquery.log";
	std::string std_path = BigQueryPythonPath.c_str();
    printf("std_path: %s\n\n",std_path.c_str());

	std::string cmd = "python " + std_path + " " + std::string(Form1->BigQueryPath.c_str()) + " \"" + std::string(start_time.c_str()) + "\" \"" + std::string(end_time.c_str()) + "\" > " + std::string(BigQueryPythonLogPath.c_str());
	printf("cmd: %s\n\n",cmd.c_str());
	int ret = system(cmd.c_str());
	
    if (ret == 0) {
        std::cout << "Get BigQuery Success\n";
    } else {
        std::cout << "Get BigQuery Fail\n";
    }
}
//-----------------------for BigQuery

//---------------------------------------------------------------------------
static const char * strnistr(const char * pszSource, DWORD dwLength, const char * pszFind)
{
	DWORD        dwIndex   = 0;
	DWORD        dwStrLen  = 0;
	const char * pszSubStr = NULL;

	// check for valid arguments
	if (!pszSource || !pszFind)
	{
		return pszSubStr;
	}

	dwStrLen = strlen(pszFind);

	// can pszSource possibly contain pszFind?
	if (dwStrLen > dwLength)
	{
		return pszSubStr;
	}

	while (dwIndex <= dwLength - dwStrLen)
	{
		if (0 == strnicmp(pszSource + dwIndex, pszFind, dwStrLen))
		{
			pszSubStr = pszSource + dwIndex;
			break;
		}

		dwIndex ++;
	}

	return pszSubStr;
}
//---------------------------------------------------------------------------
static char *stristr(const char *String, const char *Pattern)
{
  char *pptr, *sptr, *start;
  size_t  slen, plen;

  for (start = (char *)String,pptr  = (char *)Pattern,slen  = strlen(String),plen  = strlen(Pattern);
       slen >= plen;start++, slen--)
      {
            /* find start of pattern in string */
            while (toupper(*start) != toupper(*Pattern))
            {
                  start++;
                  slen--;

                  /* if pattern longer than string */

                  if (slen < plen)
                        return(NULL);
            }

            sptr = start;
            pptr = (char *)Pattern;

            while (toupper(*sptr) == toupper(*pptr))
            {
                  sptr++;
                  pptr++;

                  /* if end of pattern then pattern was found */

                  if ('\0' == *pptr)
                        return (start);
            }
      }
   return(NULL);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
  AircraftDBPathFileName=ExtractFilePath(ExtractFileDir(Application->ExeName)) +AnsiString("..\\AircraftDB\\")+AIRCRAFT_DATABASE_FILE;
  AirportDBPathFileName=ExtractFilePath(ExtractFileDir(Application->ExeName)) +AnsiString("..\\AirportDB\\")+AIRPORT_DATABASE_FILE;
  RouteDBPathFileName=ExtractFilePath(ExtractFileDir(Application->ExeName)) +AnsiString("..\\routeDB\\")+ROUTE_DATABASE_FILE;
  ARTCCBoundaryDataPathFileName=ExtractFilePath(ExtractFileDir(Application->ExeName)) +AnsiString("..\\ARTCC_Boundary_Data\\")+ARTCC_BOUNDARY_FILE;
  BigQueryPath=ExtractFilePath(ExtractFileDir(Application->ExeName)) +AnsiString("..\\BigQuery\\");
  BigQueryPythonScript= BigQueryPath+ AnsiString(BIG_QUERY_RUN_FILENAME);
  DeleteFilesWithExtension(BigQueryPath, "csv");
  BigQueryLogFileName=BigQueryPath+"BigQuery.log";
  DeleteFileA(BigQueryLogFileName.c_str());
  CurrentSpriteImage=0;
  InitDecodeRawADS_B();
  RecordRawStream=NULL;
  PlayBackRawStream=NULL;
  TrackHook.Valid_CC=false;
  TrackHook.Valid_CPA=false;

  AreaTemp=NULL;
  Areas= new TList;

 MouseDown=false;

 MapCenterLat=MAP_CENTER_LAT;
 MapCenterLon=MAP_CENTER_LON;

#ifndef YAKI_TEST_CODE // MAP_FACTORY_INIT
  RegisterMapProviders();
#endif

#ifndef YAKI_TEST_CODE
  DWORD dwFlags;
  if (!InternetGetConnectedState(&dwFlags, 0)) {
    LoadMapFromInternet=false;
  } else {
    LoadMapFromInternet=true;
  }
#else
 LoadMapFromInternet=false;
#endif
 MapComboBox->ItemIndex=GoogleMaps;
 //MapComboBox->ItemIndex=SkyVector_VFR;
 //MapComboBox->ItemIndex=SkyVector_IFR_Low;
 //MapComboBox->ItemIndex=SkyVector_IFR_High;
 LoadMap(MapComboBox->ItemIndex);
#ifndef YAKI_TEST_CODE
 LoadAirport();
 LoadRoute();
#endif
 g_EarthView->m_Eye.h /= pow(1.3,18);//pow(1.3,43);
 SetMapCenter(g_EarthView->m_Eye.x, g_EarthView->m_Eye.y);
 TimeToGoTrackBar->Position=120;
 BigQueryCSV=NULL;
 BigQueryRowCount=0;
 BigQueryFileCount=0;
 InitAircraftDB(AircraftDBPathFileName);
#ifndef YAKI_TEST_CODE
 InitAirportDB(AirportDBPathFileName);
 InitRouteDB(RouteDBPathFileName);
#endif
//    // ��Ŀ ����
#ifdef USE_WORKER_THREAD
// YAKI_TEST_CODE
    CpaCache = new TCPAResultCache();
    WorkerThread = new TCPAWorkerThread(CpaCache);
    WorkerThread->Start(); // Execute() ����
#endif
#ifndef YAKI_TEST_CODE
 mouseover_aircraft = NULL;
 mouseover_airport = NULL;
#endif
 printf("init complete\n");
}
//---------------------------------------------------------------------------
__fastcall TForm1::~TForm1()
{
 Timer1->Enabled=false;
 Timer2->Enabled=false;
 delete g_EarthView;
 if (g_GETileManager) delete g_GETileManager;
 delete g_MasterLayer;
 delete g_Storage;
 if (LoadMapFromInternet)
 {
   if (g_Keyhole) delete g_Keyhole;
 }

}
//---------------------------------------------------------------------------
void __fastcall  TForm1::SetMapCenter(double &x, double &y)
{
  double siny;
  x=(MapCenterLon+0.0)/360.0;
  siny=sin((MapCenterLat * M_PI) / 180.0);
  siny = fmin(fmax(siny, -0.9999), 0.9999);
  y=(log((1 + siny) / (1 - siny)) / (4 * M_PI));
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ObjectDisplayInit(TObject *Sender)
{
	glViewport(0,0,(GLsizei)ObjectDisplay->Width,(GLsizei)ObjectDisplay->Height);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable (GL_LINE_STIPPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    NumSpriteImages=MakeAirplaneImages();
#ifndef YAKI_TEST_CODE
    airportSpriteNum=MakeAirportImages();
#endif    
	MakeAirTrackFriend();
	MakeAirTrackHostile();
	MakeAirTrackUnknown();
	MakePoint();
	MakeTrackHook();
	g_EarthView->Resize(ObjectDisplay->Width,ObjectDisplay->Height);
	glPushAttrib (GL_LINE_BIT);
	glPopAttrib ();


    printf("OpenGL Version %s\n",glGetString(GL_VERSION));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ObjectDisplayResize(TObject *Sender)
{
	 double Value;
	//ObjectDisplay->Width=ObjectDisplay->Height;
	glViewport(0,0,(GLsizei)ObjectDisplay->Width,(GLsizei)ObjectDisplay->Height);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable (GL_LINE_STIPPLE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	g_EarthView->Resize(ObjectDisplay->Width,ObjectDisplay->Height);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ObjectDisplayPaint(TObject *Sender)
{

 if (DrawMap->Checked)glClearColor(0.0,0.0,0.0,0.0);
 else	glClearColor(BG_INTENSITY,BG_INTENSITY,BG_INTENSITY,0.0);

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 g_EarthView->Animate();
 g_EarthView->Render(DrawMap->Checked);
 g_GETileManager->Cleanup();
 Mw1 = Map_w[1].x-Map_w[0].x;
 Mw2 = Map_v[1].x-Map_v[0].x;
 Mh1 = Map_w[1].y-Map_w[0].y;
 Mh2 = Map_v[3].y-Map_v[0].y;

 xf=Mw1/Mw2;
 yf=Mh1/Mh2;

 DrawObjects();
 if(log_time_cnt == 2){
	 auto end_time = std::chrono::high_resolution_clock::now();
	 auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - g_start_time).count();
	 printf("aircraft data End: %lld ms, Aircraft info panel updated, elapsed: %lld ms\n",
		std::chrono::duration_cast<std::chrono::milliseconds>(end_time.time_since_epoch()).count(),
		elapsed);
	 log_time_cnt += 1;
 }
 if(log_time_cnt_raw == 2){
	  auto end_time = std::chrono::high_resolution_clock::now();
	  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - g_start_time_raw_connect).count();
	  printf("Raw Loading Track End: %lld ms, elapsed: %lld ms\n",
		std::chrono::duration_cast<std::chrono::milliseconds>(end_time.time_since_epoch()).count(),
		elapsed);
	  log_time_cnt_raw += 1;
 }
 if(log_time_cnt_sbs == 2){
	  auto end_time = std::chrono::high_resolution_clock::now();
	  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - g_start_time_sbs_connect).count();
	  printf("SBS Loading Track End: %lld ms, elapsed: %lld ms\n",
		std::chrono::duration_cast<std::chrono::milliseconds>(end_time.time_since_epoch()).count(),
		elapsed);
	  log_time_cnt_sbs += 1;
 }
 if(log_time_cnt_polygon == 2){
	  auto end_time = std::chrono::high_resolution_clock::now();
	  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - g_start_time_polygon).count();
	  printf("Polygon Track End: %lld ms, elapsed: %lld ms\n",
		std::chrono::duration_cast<std::chrono::milliseconds>(end_time.time_since_epoch()).count(),
		elapsed);
	  log_time_cnt_polygon += 1;
 }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
 __int64 CurrentTime;

 CurrentTime=GetCurrentTimeInMsec();
 SystemTime->Caption=TimeToChar(CurrentTime);

 ObjectDisplay->Repaint();
}
//---------------------------------------------------------------------------
#ifndef YAKI_TEST_CODE
void remove_spaces(char *str) {
    char *src = str, *dst = str;

    while (*src) {
        if (!isspace((unsigned char)*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}


inline double DegToRad(double deg) { return deg * M_PI / 180.0; }
inline double RadToDeg(double rad) { return rad * 180.0 / M_PI; }

std::vector<TLatLon> GenerateGreatCirclePoints(double lat1_deg, double lon1_deg, double lat2_deg, double lon2_deg, int steps = 100) {
    std::vector<TLatLon> points;

    double lat1 = DegToRad(lat1_deg);
    double lon1 = DegToRad(lon1_deg);
    double lat2 = DegToRad(lat2_deg);
    double lon2 = DegToRad(lon2_deg);

    // Haversine formula to compute angular distance
    double d = 2.0 * asin(sqrt(
        pow(sin((lat2 - lat1) / 2.0), 2) +
        cos(lat1) * cos(lat2) * pow(sin((lon2 - lon1) / 2.0), 2)));

    if (d == 0.0) return points;

    for (int i = 0; i <= steps; ++i) {
        double f = static_cast<double>(i) / steps;

        double A = sin((1 - f) * d) / sin(d);
        double B = sin(f * d) / sin(d);

        double x = A * cos(lat1) * cos(lon1) + B * cos(lat2) * cos(lon2);
        double y = A * cos(lat1) * sin(lon1) + B * cos(lat2) * sin(lon2);
        double z = A * sin(lat1) + B * sin(lat2);

        double lat = atan2(z, sqrt(x * x + y * y));
        double lon = atan2(y, x);

        TLatLon pt;
        pt.Latitude = RadToDeg(lat);
        pt.Longitude = RadToDeg(lon);

        // �浵 ������ -180 ~ 180 ���� ����ȭ
        if (pt.Longitude < -180.0) pt.Longitude += 360.0;
        if (pt.Longitude > 180.0) pt.Longitude -= 360.0;

        points.push_back(pt);
    }

    return points;
}

void __fastcall TForm1::split_and_print(const char *input) {
    char buffer[256];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    std::vector<AnsiString> tokenArr;

    char *token = strtok(buffer, "-");
    bool origin = true;

    // �ִ� 10�� ���ױ��� ó��
    while (token != NULL && tokenArr.size() < 10) {
        tokenArr.push_back(token);

        double latitude, longitude, ScrX, ScrY;
        TAirportData* airportData = GetAirportDBInfo(token, latitude, longitude);
        if (airportData) {
            LatLon2XY(latitude, longitude, ScrX, ScrY);

            if (origin) {
                glColor4f(0.0, 0.0, 1.0, 1.0); // �������� �Ķ���
                origin = false;
            } else {
                glColor4f(0.0, 1.0, 1.0, 1.0); // ������ �þȻ�
            }

            DrawPoint(ScrX, ScrY);
            if (true || g_EarthView->GetCurrentZoom() / 100 > 0.7f) {
                glRasterPos2i(ScrX + 30, ScrY - 10);
#ifndef YAKI_TEST_CODE
                ObjectDisplay->Draw2DText(airportData->Fields[3].c_str());
#else
                ObjectDisplay->Draw2DText(token);
#endif
            }
            glEnd();
        }

        token = strtok(NULL, "-");
    }
    FlightDepArrLabel->Caption = "";
    bool first = true;
    for (size_t i = 0; i < tokenArr.size(); i++) {
        double lat1, lon1;
    	TAirportData* airportData = GetAirportDBInfo(tokenArr[i].c_str(), lat1, lon1);
        if (airportData) {
            if (first) {
             FlightDepArrLabel->Caption += airportData->Fields[3].c_str();
             first = false;
            } else {
             FlightDepArrLabel->Caption += "-";
             FlightDepArrLabel->Caption += airportData->Fields[3].c_str();
            }
        }
	}
    // �� �� �̻��̸� �������� �����׷η� ����
    for (size_t i = 0; i + 1 < tokenArr.size(); ++i) {
        double lat1, lon1, lat2, lon2;

        if (GetAirportDBInfo(tokenArr[i].c_str(), lat1, lon1) &&
            GetAirportDBInfo(tokenArr[i + 1].c_str(), lat2, lon2)) {

            auto points = GenerateGreatCirclePoints(lat1, lon1, lat2, lon2, 100);

            // ���� ������ ����
            if (i % 3 == 0) glColor4f(1.0, 1.0, 0.0, 1.0);      // ������
            else if (i % 3 == 1) glColor4f(0.0, 1.0, 0.0, 1.0); // ����
            else glColor4f(1.0, 0.0, 1.0, 1.0);                 // ������

            glLineWidth(2.0);
            glBegin(GL_LINE_STRIP);

            for (size_t j = 0; j < points.size(); ++j) {
                double x, y;
                LatLon2XY(points[j].Latitude, points[j].Longitude, x, y);

                if (j > 0) {
                    double lon1 = points[j - 1].Longitude;
                    double lon2 = points[j].Longitude;
                    double lonDiff = fabs(lon2 - lon1);
                    if (lonDiff > 180.0) {
                        glEnd();               // ���� �� ����
                        glBegin(GL_LINE_STRIP); // ���ο� �� ����
                    }
                }

                glVertex2f(x, y);
            }

            glEnd();
        }
    }
}
#endif
void __fastcall TForm1::DrawObjects(void)
{
  double ScrX, ScrY;
  int    ViewableAircraft=0;

  glEnable( GL_LINE_SMOOTH );
  glEnable( GL_POINT_SMOOTH );
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
  glLineWidth(3.0);
  glPointSize(4.0);
  glColor4f(1.0, 1.0, 1.0, 1.0);

  LatLon2XY(MapCenterLat,MapCenterLon, ScrX, ScrY);

  glBegin(GL_LINE_STRIP);
  glVertex2f(ScrX-20.0,ScrY);
  glVertex2f(ScrX+20.0,ScrY);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f(ScrX,ScrY-20.0);
  glVertex2f(ScrX,ScrY+20.0);
  glEnd();


  uint32_t *Key;
  ght_iterator_t iterator;
  TADS_B_Aircraft* Data,*DataCPA;

  DWORD i,j,Count;
#ifndef YAKI_TEST_CODE
  bool selectedArea = false;
#endif
  if (AreaTemp)
  {
   glPointSize(3.0);
	for (DWORD i = 0; i <AreaTemp->NumPoints ; i++)
		LatLon2XY(AreaTemp->Points[i][1],AreaTemp->Points[i][0],
				  AreaTemp->PointsAdj[i][0],AreaTemp->PointsAdj[i][1]);

   glBegin(GL_POINTS);
   for (DWORD i = 0; i <AreaTemp->NumPoints ; i++)
	{
	glVertex2f(AreaTemp->PointsAdj[i][0],
			   AreaTemp->PointsAdj[i][1]);
	}
	glEnd();
   glBegin(GL_LINE_STRIP);
   for (DWORD i = 0; i <AreaTemp->NumPoints ; i++)
	{
	glVertex2f(AreaTemp->PointsAdj[i][0],
			   AreaTemp->PointsAdj[i][1]);
	}
	glEnd();
  }
	Count=Areas->Count;
	for (i = 0; i < Count; i++)
	 {
	   TArea *Area = (TArea *)Areas->Items[i];
	   TMultiColor MC;

	   MC.Rgb=ColorToRGB(Area->Color);
	   if (Area->Selected)
	   {
		  glLineWidth(4.0);
		  glPushAttrib (GL_LINE_BIT);
		  glLineStipple (3, 0xAAAA);
	   }


	   glColor4f(MC.Red/255.0, MC.Green/255.0, MC.Blue/255.0, 1.0);
	   glBegin(GL_LINE_LOOP);
	   for (j = 0; j <Area->NumPoints; j++)
	   {
		LatLon2XY(Area->Points[j][1],Area->Points[j][0], ScrX, ScrY);
		glVertex2f(ScrX,ScrY);
	   }
	  glEnd();
	   if (Area->Selected)
	   {
		glPopAttrib ();
#ifndef YAKI_TEST_CODE
		selectedArea = true;
		glLineWidth(5.0);
#else
		glLineWidth(2.0);
#endif
	   }

	   glColor4f(MC.Red/255.0, MC.Green/255.0, MC.Blue/255.0, 0.4);

	   for (j = 0; j <Area->NumPoints; j++)
	   {
		LatLon2XY(Area->Points[j][1],Area->Points[j][0],
				  Area->PointsAdj[j][0],Area->PointsAdj[j][1]);
	   }
	  TTriangles *Tri=Area->Triangles;

	  while(Tri)
	  {
		glBegin(GL_TRIANGLES);
		glVertex2f(Area->PointsAdj[Tri->indexList[0]][0],
				   Area->PointsAdj[Tri->indexList[0]][1]);
		glVertex2f(Area->PointsAdj[Tri->indexList[1]][0],
				   Area->PointsAdj[Tri->indexList[1]][1]);
		glVertex2f(Area->PointsAdj[Tri->indexList[2]][0],
				   Area->PointsAdj[Tri->indexList[2]][1]);
		glEnd();
		Tri=Tri->next;
	  }
	 }
#ifndef YAKI_TEST_CODE
    ght_hash_table_t* AirportDBHashTable = getAirportDBHashTable();
    TAirportData *airportData;
    bool firstAirportData = true;
   	for(airportData = (TAirportData *)ght_first(AirportDBHashTable, &iterator,(const void **) &Key);
			  airportData; airportData = (TAirportData *)ght_next(AirportDBHashTable, &iterator, (const void **)&Key))
	{
         if (firstAirportData) {
             firstAirportData = false;
             continue;
         }
         LatLon2XY(StrToFloat(airportData->Fields[6]),StrToFloat(airportData->Fields[7]), ScrX, ScrY);
//         glColor4f(1.0, 1.0, 0.0, 1.0);
#ifndef YAKI_TEST_CODE
         if (StrToFloat(airportData->Fields[6]) > 85.0 || StrToFloat(airportData->Fields[6]) < -85.0) {
         	continue;
         }
#endif
         DrawAirportImage(ScrX,ScrY, g_EarthView->GetCurrentZoom()/50 > 0.7 ? 0.7 : g_EarthView->GetCurrentZoom()/50 < 0.2 ? 0.2 : g_EarthView->GetCurrentZoom()/50, airportSpriteNum);
         if (airportData == mouseover_airport) {
           glRasterPos2i(ScrX+30,ScrY-10);
           ObjectDisplay->Draw2DText(airportData->Fields[3].c_str());
         }
//         DrawAirport(ScrX,ScrY, g_EarthView->GetCurrentZoom()/50 > 1.0 ? 1.0 : g_EarthView->GetCurrentZoom()/50 < 0.2 ? 0.2 : g_EarthView->GetCurrentZoom()/50);
    }
#endif
    AircraftCountLabel->Caption=IntToStr((int)AircraftManager::GetInstance()->GetSize());
	for(Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetFirst(&iterator,(const void **) &Key);
			  Data; Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetNext(&iterator, (const void **)&Key))
	{
	  if (Data->HaveLatLon)
	  {
#ifndef YAKI_TEST_CODE
          Data->visible = 0;
          if(selectedArea) {
                Count=Areas->Count;
                for (i = 0; i < Count; i++)
                {
                   TArea *Area = (TArea *)Areas->Items[i];
                   if (Area->Selected)
                   {
                       pfVec3 Point;
                       Point[0]=Data->Longitude;
                       Point[1]=Data->Latitude;
                       Point[2]=0.0;
                       if (PointInPolygon(Area->Points,Area->NumPoints,Point)) {
                           Data->visible++;
                       }
                   }
                }
          } else {
              Data->visible++;
          }
          if (Data->visible == 0) {
              continue;
          }
          ViewableAircraft++;
#endif


	   glColor4f(1.0, 1.0, 1.0, 1.0);

	   LatLon2XY(Data->Latitude,Data->Longitude, ScrX, ScrY);
	   //DrawPoint(ScrX,ScrY);
	   if (Data->HaveSpeedAndHeading)   glColor4f(1.0, 0.0, 1.0, 1.0);
	   else
		{
		 Data->Heading=0.0;
		 glColor4f(1.0, 0.0, 0.0, 1.0);
		}
#ifndef YAKI_TEST_CODE
       if (mouseover_aircraft == Data) {
            glColor4f(0.7, 0.7, 0.0, 1.0);
       }
       if (aircraft_is_helicopter(Data->ICAO, NULL)) {
       		DrawAirplaneImage(ScrX,ScrY,g_EarthView->GetCurrentZoom()/50 > 1.5 ? 1.5 : g_EarthView->GetCurrentZoom()/50 < 0.5 ? 0.5 : g_EarthView->GetCurrentZoom()/50,Data->Heading,72);
       } else if(aircraft_is_military(Data->ICAO, NULL)) {
       		DrawAirplaneImage(ScrX,ScrY,g_EarthView->GetCurrentZoom()/50 > 1.5 ? 1.5 : g_EarthView->GetCurrentZoom()/50 < 0.5 ? 0.5 : g_EarthView->GetCurrentZoom()/50,Data->Heading,27);
       } else {
       		DrawAirplaneImage(ScrX,ScrY,g_EarthView->GetCurrentZoom()/50 > 1.5 ? 1.5 : g_EarthView->GetCurrentZoom()/50 < 0.5 ? 0.5 : g_EarthView->GetCurrentZoom()/50,Data->Heading,Data->SpriteImage);
       }
#ifndef YAKI_TEST_CODE
       if (mouseover_aircraft == Data || g_EarthView->GetCurrentZoom()/100 > 0.7f) {
           glRasterPos2i(ScrX+30,ScrY-10);
           ObjectDisplay->Draw2DText(Data->HexAddr);
       }
#endif
#else
	   DrawAirplaneImage(ScrX,ScrY,1.5,Data->Heading,Data->SpriteImage);
	   glRasterPos2i(ScrX+30,ScrY-10);
	   ObjectDisplay->Draw2DText(Data->HexAddr);
#endif
	   if ((Data->HaveSpeedAndHeading) && (TimeToGoCheckBox->State==cbChecked))
	   {
		double lat,lon,az;
		if (VDirect(Data->Latitude,Data->Longitude,
					Data->Heading,Data->Speed/3060.0*TimeToGoTrackBar->Position ,&lat,&lon,&az)==OKNOERROR)
		  {
			 double ScrX2, ScrY2;
			 LatLon2XY(lat,lon, ScrX2, ScrY2);
             glColor4f(1.0, 1.0, 0.0, 1.0);
			 glBegin(GL_LINE_STRIP);
			 glVertex2f(ScrX,ScrY);
			 glVertex2f(ScrX2,ScrY2);
			 glEnd();
		  }
	   }
	 }
	}
 ViewableAircraftCountLabel->Caption=ViewableAircraft;
 if (TrackHook.Valid_CC)
 {
		Data= (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetAircraft(sizeof(TrackHook.ICAO_CC), (void *)&TrackHook.ICAO_CC);
		if (Data)
		{
		ICAOLabel->Caption=Data->HexAddr;
        if (Data->HaveFlightNum) {
          FlightNumLabel->Caption=Data->FlightNum;
 #ifndef YAKI_TEST_CODE
          remove_spaces(Data->FlightNum);
          bool bFind;
          const char *routeInfo = GetRouteDBInfo(Data->FlightNum, bFind);
          if (bFind) {
//              FlightDepArrLabel->Caption = routeInfo;
              split_and_print(routeInfo);
          } else {
              FlightDepArrLabel->Caption = "N/A";
          }

 #endif
 #ifndef YAKI_TEST_CODE
         double prevX, prevY;
        bool first = true;
         for (int i = 0; i < Data->LatLonHistory->Count; i++)
         {
             TLatLon *obj = (TLatLon*) Data->LatLonHistory->Items[i];
            // obj ����
            // Check altitude
            float r, g, b;
            if (obj->Altitude < 1000) {
                r = 0.0f; g = 0.0f; b = 1.0f; // �Ķ�
            } else if (obj->Altitude < 5000) {
                r = 0.0f; g = 1.0f; b = 1.0f; // û��
            } else if (obj->Altitude < 10000) {
                r = 0.0f; g = 1.0f; b = 0.0f; // �ʷ�
            } else if (obj->Altitude < 20000) {
                r = 1.0f; g = 1.0f; b = 0.0f; // ����
            } else {
                r = 1.0f; g = 0.0f; b = 0.0f; // ����
            }
            glColor3f(r, g, b);
 //           glColor3f(1.0f, 0.0f, 0.0f);   // ����
            glPointSize(10.0f);            // �� ũ�� 10�ȼ�
            LatLon2XY(obj->Latitude,obj->Longitude, ScrX, ScrY);
            glBegin(GL_POINTS);
            glVertex2f(ScrX, ScrY);
            glEnd();
    // �� �׸���
            if (!first)
            {
 //               glColor3f(0.0f, 1.0f, 0.0f); // �� ��: �ʷ�
                glLineWidth(2.0f);
                glBegin(GL_LINES);
                    glVertex2f(prevX, prevY);
                    glVertex2f(ScrX, ScrY);
                glEnd();
            }

            prevX = ScrX;
            prevY = ScrY;
            first = false;

         }

         // draw history
#endif
        }
        else FlightNumLabel->Caption="N/A";
        if (Data->HaveLatLon)
		{
		 CLatLabel->Caption=DMS::DegreesMinutesSecondsLat(Data->Latitude).c_str();
		 CLonLabel->Caption=DMS::DegreesMinutesSecondsLon(Data->Longitude).c_str();
        }
        else
        {
         CLatLabel->Caption="N/A";
		 CLonLabel->Caption="N/A";
        }
        if (Data->HaveSpeedAndHeading)
        {
		 SpdLabel->Caption=FloatToStrF(Data->Speed, ffFixed,12,2)+" KTS  VRATE:"+FloatToStrF(Data->VerticalRate,ffFixed,12,2);
		 HdgLabel->Caption=FloatToStrF(Data->Heading, ffFixed,12,2)+" DEG";
        }
        else
        {
 		 SpdLabel->Caption="N/A";
		 HdgLabel->Caption="N/A";
        }
        if (Data->Altitude)
		 AltLabel->Caption= FloatToStrF(Data->Altitude, ffFixed,12,2)+" FT";
		else AltLabel->Caption="N/A";

		MsgCntLabel->Caption="Raw: "+IntToStr((int)Data->NumMessagesRaw)+" SBS: "+IntToStr((int)Data->NumMessagesSBS);
		TrkLastUpdateTimeLabel->Caption=TimeToChar(Data->LastSeen);

        glColor4f(1.0, 0.0, 0.0, 1.0);
        LatLon2XY(Data->Latitude,Data->Longitude, ScrX, ScrY);
        DrawTrackHook(ScrX, ScrY, g_EarthView->GetCurrentZoom()/50 > 1.0 ? 1.0 : g_EarthView->GetCurrentZoom()/50 < 0.2 ? 0.2 : g_EarthView->GetCurrentZoom()/50);
        }

		else
        {
		 TrackHook.Valid_CC=false;
		 ICAOLabel->Caption="N/A";
		 FlightNumLabel->Caption="N/A";
         CLatLabel->Caption="N/A";
		 CLonLabel->Caption="N/A";
         SpdLabel->Caption="N/A";
		 HdgLabel->Caption="N/A";
		 AltLabel->Caption="N/A";
		 MsgCntLabel->Caption="N/A";
         TrkLastUpdateTimeLabel->Caption="N/A";
         FlightDepArrLabel->Caption = "N/A";
        }
 }
 if (TrackHook.Valid_CPA)
 {
  bool CpaDataIsValid=false;
  DataCPA= (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetAircraft(sizeof(TrackHook.ICAO_CPA), (void *)&TrackHook.ICAO_CPA);
  if ((DataCPA) && (TrackHook.Valid_CC))
	{

	  double tcpa,cpa_distance_nm, vertical_cpa;
	  double lat1, lon1,lat2, lon2, junk;
	  if(log_time_cnt_cpa == 1){
		  g_start_time_cpa = std::chrono::high_resolution_clock::now();
		  printf("Start: CPA Calculate [%lld ms]\n",
			std::chrono::duration_cast<std::chrono::milliseconds>(g_start_time_cpa.time_since_epoch()).count());
	  }

	  if (computeCPA(Data->Latitude, Data->Longitude, Data->Altitude,
					 Data->Speed,Data->Heading,
					 DataCPA->Latitude, DataCPA->Longitude, DataCPA->Altitude,
					 DataCPA->Speed,DataCPA->Heading,
					 tcpa,cpa_distance_nm, vertical_cpa))
	  {
		if (VDirect(Data->Latitude,Data->Longitude,
					Data->Heading,Data->Speed/3600.0*tcpa,&lat1,&lon1,&junk)==OKNOERROR)
		{
		  if (VDirect(DataCPA->Latitude,DataCPA->Longitude,
					  DataCPA->Heading,DataCPA->Speed/3600.0*tcpa,&lat2,&lon2,&junk)==OKNOERROR)
		   {
			 glColor4f(0.0, 1.0, 0.0, 1.0);
			 glBegin(GL_LINE_STRIP);
			 LatLon2XY(Data->Latitude,Data->Longitude, ScrX, ScrY);
			 glVertex2f(ScrX, ScrY);
			 LatLon2XY(lat1,lon1, ScrX, ScrY);
			 glVertex2f(ScrX, ScrY);
			 glEnd();
			 glBegin(GL_LINE_STRIP);
			 LatLon2XY(DataCPA->Latitude,DataCPA->Longitude, ScrX, ScrY);
			 glVertex2f(ScrX, ScrY);
			 LatLon2XY(lat2,lon2, ScrX, ScrY);
			 glVertex2f(ScrX, ScrY);
			 glEnd();
			 glColor4f(1.0, 0.0, 0.0, 1.0);
			 glBegin(GL_LINE_STRIP);
			 LatLon2XY(lat1,lon1, ScrX, ScrY);
			 glVertex2f(ScrX, ScrY);
			 LatLon2XY(lat2,lon2, ScrX, ScrY);
			 glVertex2f(ScrX, ScrY);
			 glEnd();
			 CpaTimeValue->Caption=TimeToChar(tcpa*1000);
			 CpaDistanceValue->Caption= FloatToStrF(cpa_distance_nm, ffFixed,10,2)+" NM VDIST: "+IntToStr((int)vertical_cpa)+" FT";
			 CpaDataIsValid=true;
		   }
		}
	  }
	  if(log_time_cnt_cpa == 1){
		  auto end_time = std::chrono::high_resolution_clock::now();
		  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - g_start_time_cpa).count();
		  printf("CPA data End: %lld ms, elapsed: %lld ms\n",
			std::chrono::duration_cast<std::chrono::milliseconds>(end_time.time_since_epoch()).count(),
			elapsed);
		  log_time_cnt_cpa += 1;
	  }

	}
   if (!CpaDataIsValid)
   {
	TrackHook.Valid_CPA=false;
	CpaTimeValue->Caption="None";
	CpaDistanceValue->Caption="None";
   }
 }
#ifndef USE_WORKER_THREAD
// YAKI_TEST_CODE
    HookTrackAll();
#else
    std::vector<TCpaPair> pairs;

    if (CpaCache) {
        CpaCache->Get(pairs);
    }

    for (auto &pair : pairs) {
        if (!pair.Valid) continue;

        double ScrX, ScrY;
#ifdef YAKI_TEST_CODE
        printf("Draw CPA\n");
#endif
        // �װ���1 -> CPA ��ġ
         glColor4f(0.0, 1.0, 0.0, 1.0);
         glBegin(GL_LINE_STRIP);
 		 glLineWidth(5.0);
         LatLon2XY(pair.a_Lat,pair.a_Lon, ScrX, ScrY);
         glVertex2f(ScrX, ScrY);
         LatLon2XY(pair.Lat1,pair.Lon1, ScrX, ScrY);
         glVertex2f(ScrX, ScrY);
         glEnd();
         glBegin(GL_LINE_STRIP);
 		 glLineWidth(5.0);
         LatLon2XY(pair.b_Lat,pair.b_Lon, ScrX, ScrY);
         glVertex2f(ScrX, ScrY);
         LatLon2XY(pair.Lat2,pair.Lon2, ScrX, ScrY);
         glVertex2f(ScrX, ScrY);
         glEnd();
         glColor4f(1.0, 0.0, 0.0, 1.0);
         glBegin(GL_LINE_STRIP);
 		 glLineWidth(5.0);
         LatLon2XY(pair.Lat1,pair.Lon1, ScrX, ScrY);
         glVertex2f(ScrX, ScrY);
         LatLon2XY(pair.Lat2,pair.Lon2, ScrX, ScrY);
         glVertex2f(ScrX, ScrY);
         glEnd();
   }
#endif
}

//---------------------------------------------------------------------------
void __fastcall TForm1::ObjectDisplayMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{

 if (Button==mbLeft)
   {
	if (Shift.Contains(ssCtrl))
	{

	}
	else
	{
	 g_MouseLeftDownX = X;
	 g_MouseLeftDownY = Y;
	 g_MouseDownMask |= LEFT_MOUSE_DOWN ;
	 g_EarthView->StartDrag(X, Y, NAV_DRAG_PAN);
	}
  }
 else if (Button==mbRight)
  {
  if (AreaTemp)
   {
	if (AreaTemp->NumPoints<MAX_AREA_POINTS)
	{
	  AddPoint(X, Y);
	}
	else ShowMessage("Max Area Points Reached");
   }
  else
   {
   if(log_time_cnt == 1){
	   g_start_time = std::chrono::high_resolution_clock::now();
	   printf("Start: Right-click detected on aircraft [%lld ms]\n",
		std::chrono::duration_cast<std::chrono::milliseconds>(g_start_time.time_since_epoch()).count());
	   log_time_cnt += 1;
   }
   if (Shift.Contains(ssCtrl))   HookTrack(X,Y,true);
   else  HookTrack(X,Y,false);
   }
  }

 else if (Button==mbMiddle)  ResetXYOffset();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ObjectDisplayMouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button == mbLeft) g_MouseDownMask &= ~LEFT_MOUSE_DOWN;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ObjectDisplayMouseMove(TObject *Sender,
	  TShiftState Shift, int X, int Y)
{
 int X1,Y1;
 double VLat,VLon;
 int i;
 Y1=(ObjectDisplay->Height-1)-Y;
 X1=X;
 if  ((X1>=Map_v[0].x) && (X1<=Map_v[1].x) &&
	  (Y1>=Map_v[0].y) && (Y1<=Map_v[3].y))

  {
   pfVec3 Point;
   VLat=atan(sinh(M_PI * (2 * (Map_w[1].y-(yf*(Map_v[3].y-Y1))))))*(180.0 / M_PI);
   VLon=(Map_w[1].x-(xf*(Map_v[1].x-X1)))*360.0;
   Lat->Caption=DMS::DegreesMinutesSecondsLat(VLat).c_str();
   Lon->Caption=DMS::DegreesMinutesSecondsLon(VLon).c_str();
   Point[0]=VLon;
   Point[1]=VLat;
   Point[2]=0.0;

   for (i = 0; i < Areas->Count; i++)
	 {
	   TArea *Area = (TArea *)Areas->Items[i];
	   if (PointInPolygon(Area->Points,Area->NumPoints,Point))
	   {
#if 0
		  MsgLog->Lines->Add("In Polygon "+ Area->Name);
#endif
       }
	 }
  }

  if (g_MouseDownMask & LEFT_MOUSE_DOWN)
  {
   g_EarthView->Drag(g_MouseLeftDownX, g_MouseLeftDownY, X,Y, NAV_DRAG_PAN);
   ObjectDisplay->Repaint();
  }
#ifndef YAKI_TEST_CODE
  double dlat,dlon,Range;
   uint32_t *Key;

   uint32_t Current_ICAO;
   double MinRange;
  ght_iterator_t iterator;
  TADS_B_Aircraft* Data;

#ifndef YAKI_TEST_CODE
  MinRange=8.0;
#else
  MinRange=8.0;
#endif
#endif

#ifndef YAKI_TEST_CODE
mouseover_airport = NULL;
mouseover_aircraft = NULL;
    ght_hash_table_t* AirportDBHashTable = getAirportDBHashTable();
    TAirportData *airportData;
    bool firstAirportData = true;
    char* Current_IATA = NULL;
   	for(airportData = (TAirportData *)ght_first(AirportDBHashTable, &iterator,(const void **) &Key);
			  airportData; airportData = (TAirportData *)ght_next(AirportDBHashTable, &iterator, (const void **)&Key))
	{
         if (firstAirportData) {
             firstAirportData = false;
             continue;
         }
         StrToFloat(airportData->Fields[6]),StrToFloat(airportData->Fields[7]);
       dlat= VLat-StrToFloat(airportData->Fields[6]);
       dlon= VLon-airportData->Fields[7];
       Range=sqrt(dlat*dlat+dlon*dlon);
       if (Range<MinRange)
       {
        Current_IATA=airportData->Fields[2].c_str();;
        MinRange=Range;
       }
    }
	if (MinRange< 0.2)
	{
	  mouseover_airport =(TAirportData *)GetAirportDBInfo(Current_IATA,dlat, dlon);
      return;
    }
#endif
#ifndef YAKI_TEST_CODE
  MinRange=8.0;
  for(Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetFirst(&iterator,(const void **) &Key);
			  Data; Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetNext(&iterator, (const void **)&Key))
	{
	  if (Data->HaveLatLon)
	  {
	   dlat= VLat-Data->Latitude;
	   dlon= VLon-Data->Longitude;
	   Range=sqrt(dlat*dlat+dlon*dlon);
	   if (Range<MinRange)
	   {
		Current_ICAO=Data->ICAO;
		MinRange=Range;
	   }
	  }
	}
	if (MinRange< 0.2)
	{
	  mouseover_aircraft =(TADS_B_Aircraft *)
			AircraftManager::GetInstance()->GetAircraft(sizeof(Current_ICAO), &Current_ICAO);
    }
#endif

}
//---------------------------------------------------------------------------
void __fastcall TForm1::ResetXYOffset(void)
{
 SetMapCenter(g_EarthView->m_Eye.x, g_EarthView->m_Eye.y);
 ObjectDisplay->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Exit1Click(TObject *Sender)
{
 Close();
}
//---------------------------------------------------------------------------
 void __fastcall TForm1::AddPoint(int X, int Y)
 {
  double Lat,Lon;

 if (XY2LatLon2(X,Y,Lat,Lon)==0)
 {
 	AreaTemp->Points[AreaTemp->NumPoints][1]=Lat;
	AreaTemp->Points[AreaTemp->NumPoints][0]=Lon;
	AreaTemp->Points[AreaTemp->NumPoints][2]=0.0;
	AreaTemp->NumPoints++;
	ObjectDisplay->Repaint();
 }
 }
 //---------------------------------------------------------------------------
#ifndef YAKI_TEST_CODE
 // deg -> radian ��ȯ ��ũ��
constexpr double DEG2RAD = M_PI / 180.0;

// ���: km ���� �Ÿ� ��ȯ
double Haversine(double lat1_deg, double lon1_deg, double lat2_deg, double lon2_deg)
{
    const double R = 6371.0; // ���� �ݰ� (km)

    // ����, �浵�� radian���� ��ȯ
    double lat1 = lat1_deg * DEG2RAD;
    double lon1 = lon1_deg * DEG2RAD;
    double lat2 = lat2_deg * DEG2RAD;
    double lon2 = lon2_deg * DEG2RAD;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dlon / 2) * std::sin(dlon / 2);

    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    return R * c; // km
}

// Nautical Miles(NM)�� �ٷ� ��� ������:
double HaversineNM(double lat1, double lon1, double lat2, double lon2)
{
    const double KM_TO_NM = 0.539957;
    return Haversine(lat1, lon1, lat2, lon2) * KM_TO_NM;
}
#endif
 void __fastcall TForm1::HookTrackAll()
 {
  double ScrX, ScrY;
  uint32_t *Key;
  ght_iterator_t iterator, iteratorCPA;
  TADS_B_Aircraft* Data,*DataCPA;
  for(Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetFirst(&iterator,(const void **) &Key);
			  Data; Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetNext(&iterator, (const void **)&Key))
  {
      if (!Data->HaveLatLon) {
          continue;
      }
      iteratorCPA = iterator;
      for(DataCPA = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetNext(&iteratorCPA, (const void **)&Key);
                  DataCPA; DataCPA = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetNext(&iteratorCPA, (const void **)&Key)) {
  #ifndef YAKI_TEST_CODE
	  if (DataCPA->HaveLatLon)
	  {
          double tcpa,cpa_distance_nm, vertical_cpa;
          double lat1, lon1,lat2, lon2, junk;
          if (HaversineNM(Data->Latitude, Data->Longitude,DataCPA->Latitude, DataCPA->Longitude) >= 30) {
              continue;
          }
          if (computeCPA(Data->Latitude, Data->Longitude, Data->Altitude,
                         Data->Speed,Data->Heading,
                         DataCPA->Latitude, DataCPA->Longitude, DataCPA->Altitude,
                         DataCPA->Speed,DataCPA->Heading,
                         tcpa,cpa_distance_nm, vertical_cpa)) 
          {
#ifndef YAKI_TEST_CODE
            if (tcpa >= 300.0) {
                continue;
            }
#endif
#define KM_TO_NM 0.539957    // Convert km to nautical miles
#define FEET_TO_KM 0.0003048 // Convert feet to km
            vertical_cpa = vertical_cpa * FEET_TO_KM * KM_TO_NM;
            double dist = std::sqrt(cpa_distance_nm * cpa_distance_nm + vertical_cpa * vertical_cpa);          
            if (!std::isnan(dist) && dist >= 1.0f) {
               continue;
                
            }
            if (VDirect(Data->Latitude,Data->Longitude,
                        Data->Heading,Data->Speed/3600.0*tcpa,&lat1,&lon1,&junk)==OKNOERROR)
            {
              if (VDirect(DataCPA->Latitude,DataCPA->Longitude,
                          DataCPA->Heading,DataCPA->Speed/3600.0*tcpa,&lat2,&lon2,&junk)==OKNOERROR)
               {
                 glColor4f(0.0, 1.0, 0.0, 1.0);
                 glLineWidth(4.0f); // YAKI_TEST_CODE
                 glBegin(GL_LINE_STRIP);
                 LatLon2XY(Data->Latitude,Data->Longitude, ScrX, ScrY);
                 glVertex2f(ScrX, ScrY);
                 LatLon2XY(lat1,lon1, ScrX, ScrY);
                 glVertex2f(ScrX, ScrY);
                 glEnd();
                 glBegin(GL_LINE_STRIP);
                 LatLon2XY(DataCPA->Latitude,DataCPA->Longitude, ScrX, ScrY);
                 glVertex2f(ScrX, ScrY);
                 LatLon2XY(lat2,lon2, ScrX, ScrY);
                 glVertex2f(ScrX, ScrY);
                 glEnd();
                 glColor4f(1.0, 0.0, 0.0, 1.0);
//                 glLineWidth(10.0f); // YAKI_TEST_CODE
                 glBegin(GL_LINE_STRIP);
                 LatLon2XY(lat1,lon1, ScrX, ScrY);
                 glVertex2f(ScrX, ScrY);
                 LatLon2XY(lat2,lon2, ScrX, ScrY);
                 glVertex2f(ScrX, ScrY);
                 glEnd();
#ifdef YAKI_TEST_CODE
                 SetMapCenter((lat1 + lat2) / 2.0, (lon1 + lon2) / 2.0);
#endif
               }
            }
#ifdef YAKI_TEST_CODE
			ShowMessage("Warning: Possible collision within 5 minutes!\n");
#endif
          }
      }
  #endif
      //printf("HookTrackAll\n");
    }
  }
 }
//---------------------------------------------------------------------------
 void __fastcall TForm1::HookTrack(int X, int Y,bool CPA_Hook)
 {
  double VLat,VLon, dlat,dlon,Range;
  int X1,Y1;
   uint32_t *Key;

   uint32_t Current_ICAO;
   double MinRange;
  ght_iterator_t iterator;
  TADS_B_Aircraft* Data;

  Y1=(ObjectDisplay->Height-1)-Y;
  X1=X;

  if  ((X1<Map_v[0].x) || (X1>Map_v[1].x) ||
	   (Y1<Map_v[0].y) || (Y1>Map_v[3].y)) return;

  VLat=atan(sinh(M_PI * (2 * (Map_w[1].y-(yf*(Map_v[3].y-Y1))))))*(180.0 / M_PI);
  VLon=(Map_w[1].x-(xf*(Map_v[1].x-X1)))*360.0;

#ifndef YAKI_TEST_CODE
  MinRange=16.0;
  Form2->Hide();
#else
  MinRange=8.0;
  Form2->Hide();
#endif
  for(Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetFirst(&iterator,(const void **) &Key);
			  Data; Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetNext(&iterator, (const void **)&Key))
	{
	  if (Data->HaveLatLon && Data->visible)
	  {
	   dlat= VLat-Data->Latitude;
	   dlon= VLon-Data->Longitude;
	   Range=sqrt(dlat*dlat+dlon*dlon);
	   if (Range<MinRange)
	   {
		Current_ICAO=Data->ICAO;
		MinRange=Range;
	   }
	  }
	}
	if (MinRange< 0.2)
	{
	  TADS_B_Aircraft * ADS_B_Aircraft =(TADS_B_Aircraft *)
			AircraftManager::GetInstance()->GetAircraft(sizeof(Current_ICAO), &Current_ICAO);
	  if (ADS_B_Aircraft)
	  {
		if (!CPA_Hook)
		{
		 TrackHook.Valid_CC=true;
		 TrackHook.ICAO_CC=ADS_B_Aircraft->ICAO;
		 printf("%s\n\n",GetAircraftDBInfo(ADS_B_Aircraft->ICAO));
#ifndef YAKI_TEST_CODE
// https://www.planespotters.net/search?q=VH-VKB
            String reg = GetAircraftDBReg(ADS_B_Aircraft->ICAO);
            if (reg == "" || reg == "?") {
            } else {
				String url = "https://www.flightradar24.com/data/aircraft/" + reg;
				//String url = "https://www.planespotters.net/search?q=VH-VKB";
				//String url = "https://www.bing.com";
				//String url = "https://globe.adsbexchange.com/?icao=" + reg;
				Form2->Show();
                Form2->WebBrowser1->Silent = true;
				//Form2->EdgeBrowser1->AdditionalBrowserArguments = L"--user-agent=\"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36\"";
				Form2->WebBrowser1->Navigate(WideString(url).c_bstr());
                //Form2->EdgeBrowser1->Navigate(WideString(url).c_bstr());
            }
#endif
		}
		else
		{
		 TrackHook.Valid_CPA=true;
		 TrackHook.ICAO_CPA=ADS_B_Aircraft->ICAO;
        }
;
	  }

	}
	else
		{
		 if (!CPA_Hook)
		  {
		   TrackHook.Valid_CC=false;
           ICAOLabel->Caption="N/A";
		   FlightNumLabel->Caption="N/A";
		   CLatLabel->Caption="N/A";
		   CLonLabel->Caption="N/A";
		   SpdLabel->Caption="N/A";
		   HdgLabel->Caption="N/A";
		   AltLabel->Caption="N/A";
		   MsgCntLabel->Caption="N/A";
		   TrkLastUpdateTimeLabel->Caption="N/A";
           FlightDepArrLabel->Caption = "N/A";
		  }
		 else
		   {
			TrackHook.Valid_CPA=false;
			CpaTimeValue->Caption="None";
	        CpaDistanceValue->Caption="None";
           }
		}

 }
//---------------------------------------------------------------------------
void __fastcall TForm1::LatLon2XY(double lat,double lon, double &x, double &y)
{
 x=(Map_v[1].x-((Map_w[1].x-(lon/360.0))/xf));
 y= Map_v[3].y- (Map_w[1].y/yf)+ (asinh(tan(lat*M_PI/180.0))/(2*M_PI*yf));
}
//---------------------------------------------------------------------------
int __fastcall TForm1::XY2LatLon2(int x, int y,double &lat,double &lon )
{
  double Lat,Lon, dlat,dlon,Range;
  int X1,Y1;

  Y1=(ObjectDisplay->Height-1)-y;
  X1=x;

  if  ((X1<Map_v[0].x) || (X1>Map_v[1].x) ||
	   (Y1<Map_v[0].y) || (Y1>Map_v[3].y)) return -1;

  lat=atan(sinh(M_PI * (2 * (Map_w[1].y-(yf*(Map_v[3].y-Y1))))))*(180.0 / M_PI);
  lon=(Map_w[1].x-(xf*(Map_v[1].x-X1)))*360.0;

  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ZoomInClick(TObject *Sender)
{
  g_EarthView->SingleMovement(NAV_ZOOM_IN);
  ObjectDisplay->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ZoomOutClick(TObject *Sender)
{
 g_EarthView->SingleMovement(NAV_ZOOM_OUT);

 ObjectDisplay->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Purge(void)
{
  uint32_t *Key;
  ght_iterator_t iterator;
  TADS_B_Aircraft* Data;
  void *p;
  __int64 CurrentTime=GetCurrentTimeInMsec();
  __int64  StaleTimeInMs=CSpinStaleTime->Value*1000;

  if (PurgeStale->Checked==false) return;

  for(Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetFirst(&iterator,(const void **) &Key);
			  Data; Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetNext(&iterator, (const void **)&Key))
	{
	  if ((CurrentTime-Data->LastSeen)>=StaleTimeInMs)
	  {
          bool defer;
          p = AircraftManager::GetInstance()->Remove(sizeof(*Key), Key, defer);
          if (!p) {
#ifndef YAKI_TEST_CODE
            printf("Removing the current iterated entry failed! This is a BUG\n");
#else
            ShowMessage("Removing the current iterated entry failed! This is a BUG\n");
#endif
          }
          if (defer == false) {
#ifndef YAKI_TEST_CODE
             for (int i = 0; i < Data->LatLonHistory->Count; i++)
             {
                 TLatLon *obj = (TLatLon*) Data->LatLonHistory->Items[i];
                 delete obj;
             }
             Data->LatLonHistory->Clear();
             delete Data->LatLonHistory;
#endif
			  delete Data;
          }

	  }
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
 Purge();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PurgeButtonClick(TObject *Sender)
{
  uint32_t *Key;
  ght_iterator_t iterator;
  TADS_B_Aircraft* Data;
  void *p;
  bool defer;
  for(Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetFirst(&iterator,(const void **) &Key);
			  Data; Data = (TADS_B_Aircraft *)AircraftManager::GetInstance()->GetNext(&iterator, (const void **)&Key))
	{

	  p = AircraftManager::GetInstance()->Remove(sizeof(*Key), Key, defer);
	  if (!p)
		ShowMessage("Removing the current iterated entry failed! This is a BUG\n");
      if (defer == false) {
#ifndef YAKI_TEST_CODE
             for (int i = 0; i < Data->LatLonHistory->Count; i++)
             {
                 TLatLon *obj = (TLatLon*) Data->LatLonHistory->Items[i];
                 delete obj;
             }
             Data->LatLonHistory->Clear();
             delete Data->LatLonHistory;
#endif
	    delete Data;
      }
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::InsertClick(TObject *Sender)
{
 Insert->Enabled=false;
 LoadARTCCBoundaries1->Enabled=false;
 Complete->Enabled=true;
 Cancel->Enabled=true;
 //Delete->Enabled=false;

 AreaTemp= new TArea;
 AreaTemp->NumPoints=0;
 AreaTemp->Name="";
 AreaTemp->Selected=false;
 AreaTemp->Triangles=NULL;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::CancelClick(TObject *Sender)
{
 TArea *Temp;
 Temp= AreaTemp;
 AreaTemp=NULL;
 delete  Temp;
 Insert->Enabled=true;
 Complete->Enabled=false;
 Cancel->Enabled=false;
 LoadARTCCBoundaries1->Enabled=true;
 //if (Areas->Count>0)  Delete->Enabled=true;
 //else   Delete->Enabled=false;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::CompleteClick(TObject *Sender)
{

  int or1=orientation2D_Polygon( AreaTemp->Points,AreaTemp->NumPoints);
  if (or1==0)
   {
	ShowMessage("Degenerate Polygon");
    CancelClick(NULL);
	return;
   }
  if (or1==CLOCKWISE)
  {
	DWORD i;

	memcpy(AreaTemp->PointsAdj,AreaTemp->Points,sizeof(AreaTemp->Points));
	for (i = 0; i <AreaTemp->NumPoints; i++)
	 {
	   memcpy(AreaTemp->Points[i],
			 AreaTemp->PointsAdj[AreaTemp->NumPoints-1-i],sizeof( pfVec3));
	 }
  }
  if (checkComplex( AreaTemp->Points,AreaTemp->NumPoints))
   {
	ShowMessage("Polygon is Complex");
	CancelClick(NULL);
	return;
   }

  AreaConfirm->ShowDialog();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AreaListViewSelectItem(TObject *Sender, TListItem *Item,
      bool Selected)
{
   DWORD Count;
   TArea *AreaS=(TArea *)Item->Data;
   bool HaveSelected=false;
    AreaS->Selected = Selected;
	Count=Areas->Count;
	for (unsigned int i = 0; i < Count; i++)
	 {
	   TArea *Area = (TArea *)Areas->Items[i];
		if (Area->Selected)
		{
		 HaveSelected=true;
		}
	 }

	if (HaveSelected)  {
		Delete->Enabled=true;
		if(log_time_cnt_polygon == 1){
			g_start_time_polygon = std::chrono::high_resolution_clock::now();
			printf("Start: Polygon Show [%lld ms]\n",
			  std::chrono::duration_cast<std::chrono::milliseconds>(g_start_time_polygon.time_since_epoch()).count());
            log_time_cnt_polygon += 1;
		}
	}
	else Delete->Enabled=false;
#ifndef YAKI_TEST_CODE
    // interested aircraft
    // 
    if (CpaCache) {
        CpaCache->Clear();
    }
#else
	ObjectDisplay->Repaint();
#endif
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DeleteClick(TObject *Sender)
{
 int i = 0;

 while (i < AreaListView->Items->Count)
  {
	if (AreaListView->Items->Item[i]->Selected)
	{
	 TArea *Area;
	 int Index;

	 Area=(TArea *)AreaListView->Items->Item[i]->Data;
	 for (Index = 0; Index < Areas->Count; Index++)
	 {
	  if (Area==Areas->Items[Index])
	  {
	   Areas->Delete(Index);
	   AreaListView->Items->Item[i]->Delete();
	   TTriangles *Tri=Area->Triangles;
	   while(Tri)
	   {
		TTriangles *temp=Tri;
		Tri=Tri->next;
		free(temp->indexList);
		free(temp);
	   }
	   delete Area;
	   break;
	  }
	 }
	}
	else
	{
	  ++i;
	}
  }
 //if (Areas->Count>0)  Delete->Enabled=true;
 //else   Delete->Enabled=false;

 ObjectDisplay->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AreaListViewCustomDrawItem(TCustomListView *Sender,
	  TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
   TRect   R;
   int Left;
  AreaListView->Canvas->Brush->Color = AreaListView->Color;
  AreaListView->Canvas->Font->Color = AreaListView->Font->Color;
  R=Item->DisplayRect(drBounds);
  AreaListView->Canvas->FillRect(R);

  AreaListView->Canvas->TextWidth(Item->Caption);

 AreaListView->Canvas->TextOut(2, R.Top, Item->Caption );

 Left = AreaListView->Column[0]->Width;

  for(int   i=0   ;i<Item->SubItems->Count;i++)
	 {
	  R=Item->DisplayRect(drBounds);
	  R.Left=R.Left+Left;
	   TArea *Area=(TArea *)Item->Data;
	  AreaListView->Canvas->Brush->Color=Area->Color;
	  AreaListView->Canvas->FillRect(R);
	 }

  if (Item->Selected)
	 {
	  R=Item->DisplayRect(drBounds);
	  AreaListView->Canvas->DrawFocusRect(R);
	 }
   DefaultDraw=false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DeleteAllAreas(void)
{
 int i = 0;

 while (AreaListView->Items->Count)
  {

	 TArea *Area;
	 int Index;

	 Area=(TArea *)AreaListView->Items->Item[i]->Data;
	 for (Index = 0; Index < Areas->Count; Index++)
	 {
	  if (Area==Areas->Items[Index])
	  {
	   Areas->Delete(Index);
	   AreaListView->Items->Item[i]->Delete();
	   TTriangles *Tri=Area->Triangles;
	   while(Tri)
	   {
		TTriangles *temp=Tri;
		Tri=Tri->next;
		free(temp->indexList);
		free(temp);
	   }
	   delete Area;
	   break;
	  }
	 }
  }

 ObjectDisplay->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormMouseWheel(TObject *Sender, TShiftState Shift,
	  int WheelDelta, TPoint &MousePos, bool &Handled)
{
 if (WheelDelta>0)
	  g_EarthView->SingleMovement(NAV_ZOOM_IN);
 else g_EarthView->SingleMovement(NAV_ZOOM_OUT);
  ObjectDisplay->Repaint();
}                                  
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TTCPClientRawHandleThread::HandleInput(void)
{
  modeS_message mm;
  TDecodeStatus Status;

 // Form1->MsgLog->Lines->Add(StringMsgBuffer);
  if (Form1->RecordRawStream)
  {
   __int64 CurrentTime;
   CurrentTime=GetCurrentTimeInMsec();
   Form1->RecordRawStream->WriteLine(IntToStr(CurrentTime));
   Form1->RecordRawStream->WriteLine(StringMsgBuffer);
  }

  Status=decode_RAW_message(StringMsgBuffer, &mm);
  if (Status==HaveMsg)
  {
   TADS_B_Aircraft *ADS_B_Aircraft;
   uint32_t addr;

	addr = (mm.AA[0] << 16) | (mm.AA[1] << 8) | mm.AA[2];


	//ADS_B_Aircraft =(TADS_B_Aircraft *) ght_get(Form1->HashTable,sizeof(addr),&addr);
    ADS_B_Aircraft =(TADS_B_Aircraft *) AircraftManager::GetInstance()->GetAircraft(sizeof(addr), &addr);
	if (ADS_B_Aircraft)
	  {
      	//Form1->MsgLog->Lines->Add("Retrived");
      }
    else
	  {
  	   ADS_B_Aircraft= new TADS_B_Aircraft;
       ADS_B_Aircraft->LatLonHistory = new TList;
	   ADS_B_Aircraft->ICAO=addr;
	   snprintf(ADS_B_Aircraft->HexAddr,sizeof(ADS_B_Aircraft->HexAddr),"%06X",(int)addr);
	   ADS_B_Aircraft->NumMessagesSBS=0;
       ADS_B_Aircraft->NumMessagesRaw=0;
       ADS_B_Aircraft->VerticalRate=0;
	   ADS_B_Aircraft->HaveAltitude=false;
       ADS_B_Aircraft->HaveLatLon=false;
	   ADS_B_Aircraft->HaveSpeedAndHeading=false;
	   ADS_B_Aircraft->HaveFlightNum=false;
	   ADS_B_Aircraft->SpriteImage=Form1->CurrentSpriteImage;
#ifndef YAKI_TEST_CODE
	   ADS_B_Aircraft->visible=false;
#endif
	   if (Form1->CycleImages->Checked)
		 Form1->CurrentSpriteImage=(Form1->CurrentSpriteImage+1)%Form1->NumSpriteImages;
	   if (AircraftManager::GetInstance()->Insert(ADS_B_Aircraft, sizeof(addr), &addr) < 0)
		  {
			printf("ght_insert Error - Should Not Happen\n");
		  }
	  }

	  RawToAircraft(&mm,ADS_B_Aircraft);
  }
  else  printf("Raw Decode Error:%d\n",Status);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::RawConnectButtonClick(TObject *Sender)
{
 IdTCPClientRaw->Host=RawIpAddress->Text;
 IdTCPClientRaw->Port=30002;
#ifndef YAKI_TEST_CODE
    DWORD dwFlags;
    if (Sender != NULL && !InternetGetConnectedState(&dwFlags, 0)) {
        ShowMessage("Check internet connection\n");
        return;
    }
#endif
 if ((RawConnectButton->Caption=="Raw Connect") && (Sender!=NULL))
 {
  try
   {
   IdTCPClientRaw->Connect();
   TCPClientRawHandleThread = new TTCPClientRawHandleThread(true);
   TCPClientRawHandleThread->UseFileInsteadOfNetwork=false;
   TCPClientRawHandleThread->FreeOnTerminate=TRUE;
   TCPClientRawHandleThread->Resume();
   }
   catch (const EIdException& e)
   {
    ShowMessage("Error while connecting: "+e.Message);
   }
 }
 else
  {
	TCPClientRawHandleThread->Terminate();
	IdTCPClientRaw->Disconnect();
	IdTCPClientRaw->IOHandler->InputBuffer->Clear();
	RawConnectButton->Caption="Raw Connect";

	SBSConnectButton->Enabled=true;
	SBSPlaybackButton->Enabled=true;
	SBSRecordButton->Enabled=true;
	RawConnectButton->Enabled=true;
	RawPlaybackButton->Enabled=true;
	RawRecordButton->Enabled=true;
	BigQueryPlayback->Enabled=true;
  }
 }
//---------------------------------------------------------------------------
void __fastcall TForm1::IdTCPClientRawConnected(TObject *Sender)
{
   //SetKeepAliveValues(const AEnabled: Boolean; const ATimeMS, AInterval: Integer);
   IdTCPClientRaw->Socket->Binding->SetKeepAliveValues(true,60*1000,15*1000);
   RawConnectButton->Caption="Raw Disconnect";

	SBSConnectButton->Enabled=false;
	SBSPlaybackButton->Enabled=false;
	SBSRecordButton->Enabled=false;

	RawPlaybackButton->Enabled=false;

	BigQueryPlayback->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::IdTCPClientRawDisconnected(TObject *Sender)
{
  TCPClientRawHandleThread->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::RawRecordButtonClick(TObject *Sender)
{
 if (RawRecordButton->Caption=="Raw Record")
 {
  if (RecordRawSaveDialog->Execute())
   {
	// First, check if the file exists.
	if (FileExists(RecordRawSaveDialog->FileName))
	  ShowMessage("File "+RecordRawSaveDialog->FileName+"already exists. Cannot overwrite.");
	else
	{
		// Open a file for writing. Creates the file if it doesn't exist, or overwrites it if it does.
	RecordRawStream= new TStreamWriter(RecordRawSaveDialog->FileName, false);
	if (RecordRawStream==NULL)
	  {
		ShowMessage("Cannot Open File "+RecordRawSaveDialog->FileName);
	  }
	 else RawRecordButton->Caption="Stop Raw Recording";
	}
  }
 }
 else
 {
   delete RecordRawStream;
   RecordRawStream=NULL;
   RawRecordButton->Caption="Raw Record";
 }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::RawPlaybackButtonClick(TObject *Sender)
{
  if ((RawPlaybackButton->Caption=="Raw Playback") && (Sender!=NULL))
 {
  if (PlaybackRawDialog->Execute())
   {
	// First, check if the file exists.
	if (!FileExists(PlaybackRawDialog->FileName))
	  ShowMessage("File "+PlaybackRawDialog->FileName+" does not exist");
	else
	{
		// Open a file for writing. Creates the file if it doesn't exist, or overwrites it if it does.
	PlayBackRawStream= new TStreamReader(PlaybackRawDialog->FileName);
	if (PlayBackRawStream==NULL)
	  {
		ShowMessage("Cannot Open File "+PlaybackRawDialog->FileName);
	  }
	 else {
		   TCPClientRawHandleThread = new TTCPClientRawHandleThread(true);
		   TCPClientRawHandleThread->UseFileInsteadOfNetwork=true;
		   TCPClientRawHandleThread->First=true;
		   TCPClientRawHandleThread->FreeOnTerminate=TRUE;
		   TCPClientRawHandleThread->Resume();
		   RawPlaybackButton->Caption="Stop Raw Playback";
         
		   
			SBSConnectButton->Enabled=false;
			SBSPlaybackButton->Enabled=false;
			SBSRecordButton->Enabled=false;
			RawConnectButton->Enabled=false;

			RawRecordButton->Enabled=false;
			BigQueryPlayback->Enabled=false;
		   
		  }
	}
  }
 }
 else
 {
	TCPClientRawHandleThread->Terminate();
	delete PlayBackRawStream;
	PlayBackRawStream=NULL;
	RawPlaybackButton->Caption="Raw Playback";

	SBSConnectButton->Enabled=true;
	SBSPlaybackButton->Enabled=true;
	SBSRecordButton->Enabled=true;
	RawConnectButton->Enabled=true;
	RawPlaybackButton->Enabled=true;
	RawRecordButton->Enabled=true;
	BigQueryPlayback->Enabled=true;
 }
}
//---------------------------------------------------------------------------
// Constructor for the thread class
__fastcall TTCPClientRawHandleThread::TTCPClientRawHandleThread(bool value) : TThread(value)
{
	FreeOnTerminate = true; // Automatically free the thread object after execution
}
//---------------------------------------------------------------------------
// Destructor for the thread class
__fastcall TTCPClientRawHandleThread::~TTCPClientRawHandleThread()
{
	// Clean up resources if needed
}
//---------------------------------------------------------------------------
// Execute method where the thread's logic resides
void __fastcall TTCPClientRawHandleThread::Execute(void)
{
  __int64 Time,SleepTime;
  while (!Terminated)
  {
	if (!UseFileInsteadOfNetwork)
	 {
	  try {
		   Form1->IdTCPClientRaw->IOHandler->ReadTimeout = 5000;  // 5초
		   if (!Form1->IdTCPClientRaw->Connected()) Terminate();
		   StringMsgBuffer=Form1->IdTCPClientRaw->IOHandler->ReadLn();
		   if(log_time_cnt_raw == 1){
			   g_start_time_raw_connect = std::chrono::high_resolution_clock::now();
			   printf("Start: RawConnect [%lld ms]\n",
			   std::chrono::duration_cast<std::chrono::milliseconds>(g_start_time_raw_connect.time_since_epoch()).count());
			   log_time_cnt_raw += 1;
		   }

			DWORD dwFlags;
			if (!InternetGetConnectedState(&dwFlags, 0)) {
				ShowMessage("Check internet connection\n");
				 Form1->PurgeButtonClick(NULL);
				 TThread::Synchronize(StopTCPClient);
				 break;
			}
		  }
	   catch (...)
		{
		 TThread::Synchronize(StopTCPClient);
		 break;
		}

	 }
	 else
	 {
	  try
        {
         if (Form1->PlayBackRawStream->EndOfStream)
           {
            printf("End Raw Playback 1\n");
            TThread::Synchronize(StopPlayback);
            break;
           }
		 StringMsgBuffer= Form1->PlayBackRawStream->ReadLine();
         Time=StrToInt64(StringMsgBuffer);
		 if (First)
	      {
		   First=false;
		   LastTime=Time;
		  }
		 SleepTime=Time-LastTime;
		 LastTime=Time;
		 if (SleepTime>0) Sleep(SleepTime);
         if (Form1->PlayBackRawStream->EndOfStream)
           {
            printf("End Raw Playback 2\n");
            TThread::Synchronize(StopPlayback);
            break;
           }
		 StringMsgBuffer= Form1->PlayBackRawStream->ReadLine();
		}
        catch (...)
		{
         printf("Raw Playback Exception\n");
		 TThread::Synchronize(StopPlayback);
		 break;
		}
	   }
     try
      {
	   // Synchronize method to safely access UI components
	   TThread::Synchronize(HandleInput);
      }
	 catch (...)
     {
      ShowMessage("TTCPClientRawHandleThread::Execute Exception 3");
	 }
  }
}
//---------------------------------------------------------------------------
void __fastcall TTCPClientRawHandleThread::StopPlayback(void)
{
 Form1->RawPlaybackButtonClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TTCPClientRawHandleThread::StopTCPClient(void)
{
 Form1->RawConnectButtonClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CycleImagesClick(TObject *Sender)
{
 CurrentSpriteImage=0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SBSConnectButtonClick(TObject *Sender)
{
 IdTCPClientSBS->Host=SBSIpAddress->Text;
 IdTCPClientSBS->Port=5002;

#ifndef YAKI_TEST_CODE
	DWORD dwFlags;
	if (Sender != NULL && !InternetGetConnectedState(&dwFlags, 0)) {
        ShowMessage("Check internet connection\n");
        return;
    }
#endif
 if ((SBSConnectButton->Caption=="SBS Connect") && (Sender!=NULL))
 {
  try
   {
   IdTCPClientSBS->Connect();
   TCPClientSBSHandleThread = new TTCPClientSBSHandleThread(true);
   TCPClientSBSHandleThread->UseFileInsteadOfNetwork=false;
   TCPClientSBSHandleThread->FreeOnTerminate=TRUE;
   TCPClientSBSHandleThread->Resume();
   }
   catch (const EIdException& e)
   {
	ShowMessage("Error while connecting: "+e.Message);
   }
 }
 else
  {
	TCPClientSBSHandleThread->Terminate();
	IdTCPClientSBS->Disconnect();
    IdTCPClientSBS->IOHandler->InputBuffer->Clear();
	SBSConnectButton->Caption="SBS Connect";

	SBSConnectButton->Enabled=true;
	SBSPlaybackButton->Enabled=true;
	SBSRecordButton->Enabled=true;
	RawConnectButton->Enabled=true;
	RawPlaybackButton->Enabled=true;
	RawRecordButton->Enabled=true;
	BigQueryPlayback->Enabled=true;

  }

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TTCPClientSBSHandleThread::HandleInput(void)
{
  modeS_message mm;
  TDecodeStatus Status;

 // Form1->MsgLog->Lines->Add(StringMsgBuffer);
  if (Form1->RecordSBSStream)
  {
   __int64 CurrentTime;
   CurrentTime=GetCurrentTimeInMsec();
   Form1->RecordSBSStream->WriteLine(IntToStr(CurrentTime));
   Form1->RecordSBSStream->WriteLine(StringMsgBuffer);
  }

  if (Form1->BigQueryCSV)
  {
    Form1->BigQueryCSV->WriteLine(StringMsgBuffer);
    Form1->BigQueryRowCount++;
	if (Form1->BigQueryRowCount>=BIG_QUERY_UPLOAD_COUNT)
	{
	 Form1->CloseBigQueryCSV();
	 //printf("string is:%s\n", Form1->BigQueryPythonScript.c_str());
	 RunPythonScript(Form1->BigQueryPythonScript,Form1->BigQueryPath+" "+Form1->BigQueryCSVFileName);
	 Form1->CreateBigQueryCSV();
	}
  }
  SBS_Message_Decode( StringMsgBuffer.c_str());

}
//---------------------------------------------------------------------------
// Constructor for the thread class
__fastcall TTCPClientSBSHandleThread::TTCPClientSBSHandleThread(bool value) : TThread(value)
{
	FreeOnTerminate = true; // Automatically free the thread object after execution
}
//---------------------------------------------------------------------------
// Destructor for the thread class
__fastcall TTCPClientSBSHandleThread::~TTCPClientSBSHandleThread()
{
	// Clean up resources if needed
}
//---------------------------------------------------------------------------
// Execute method where the thread's logic resides
void __fastcall TTCPClientSBSHandleThread::Execute(void)
{
  __int64 Time,SleepTime;
  while (!Terminated)
  {
	if (!UseFileInsteadOfNetwork)
	 {
	  try {
           Form1->IdTCPClientSBS->IOHandler->ReadTimeout = 5000;  // 5초
		   if (!Form1->IdTCPClientSBS->Connected())	Terminate();
		   if (Form1->IdTCPClientSBS) {
			   StringMsgBuffer=Form1->IdTCPClientSBS->IOHandler->ReadLn();
			   if(log_time_cnt_sbs == 1){
			       g_start_time_sbs_connect = std::chrono::high_resolution_clock::now();
				   printf("Start: SBS Connect [%lld ms]\n",
					std::chrono::duration_cast<std::chrono::milliseconds>(g_start_time_sbs_connect.time_since_epoch()).count());
                   log_time_cnt_sbs += 1;
			   }

		   }
			DWORD dwFlags;
			if (!InternetGetConnectedState(&dwFlags, 0)) {
				ShowMessage("Check internet connection\n");
				 Form1->PurgeButtonClick(NULL);
				 TThread::Synchronize(StopTCPClient);
				 break;
			}
		  }
	   catch (...)
		{
		 TThread::Synchronize(StopTCPClient);
		 break;
		}

	 }
	 else
	 {
	  try
        {
         if (Form1->PlayBackSBSStream->EndOfStream)
           {
            printf("End SBS Playback 1\n");
            TThread::Synchronize(StopPlayback);
            break;
           }
		 StringMsgBuffer= Form1->PlayBackSBSStream->ReadLine();
         Time=StrToInt64(StringMsgBuffer);
		 if (First)
	      {
		   First=false;
		   LastTime=Time;
		  }
		 SleepTime=Time-LastTime;
		 LastTime=Time;
		 if (SleepTime>0) Sleep(SleepTime);
         if (Form1->PlayBackSBSStream && Form1->PlayBackSBSStream->EndOfStream)
           {
            printf("End SBS Playback 2\n");
            TThread::Synchronize(StopPlayback);
            break;
           }
           if (Form1->PlayBackSBSStream) {
			 StringMsgBuffer= Form1->PlayBackSBSStream->ReadLine();
           }
		}
        catch (...)
		{
         printf("SBS Playback Exception\n");
		 TThread::Synchronize(StopPlayback);
		 break;
		}
	   }
     try
      {
	   // Synchronize method to safely access UI components
	   TThread::Synchronize(HandleInput);
      }
	 catch (...)
     {
      ShowMessage("TTCPClientSBSHandleThread::Execute Exception 3");
	 }
  }
}
//---------------------------------------------------------------------------
void __fastcall TTCPClientSBSHandleThread::StopPlayback(void)
{
 Form1->SBSPlaybackButtonClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TTCPClientSBSHandleThread::StopTCPClient(void)
{
 Form1->SBSConnectButtonClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SBSRecordButtonClick(TObject *Sender)
{
 if (SBSRecordButton->Caption=="SBS Record")
 {
  if (RecordSBSSaveDialog->Execute())
   {
	// First, check if the file exists.
	if (FileExists(RecordSBSSaveDialog->FileName))
	  ShowMessage("File "+RecordSBSSaveDialog->FileName+"already exists. Cannot overwrite.");
	else
	{
		// Open a file for writing. Creates the file if it doesn't exist, or overwrites it if it does.
	RecordSBSStream= new TStreamWriter(RecordSBSSaveDialog->FileName, false);
	if (RecordSBSStream==NULL)
	  {
		ShowMessage("Cannot Open File "+RecordSBSSaveDialog->FileName);
	  }
	 else SBSRecordButton->Caption="Stop SBS Recording";
	}
  }
 }
 else
 {
   delete RecordSBSStream;
   RecordSBSStream=NULL;
   SBSRecordButton->Caption="SBS Record";
 }

}
//---------------------------------------------------------------------------
void __fastcall TForm1::SBSPlaybackButtonClick(TObject *Sender)
{
/*
  if ((SBSPlaybackButton->Caption=="SBS Playback") && (Sender!=NULL))
  {

	std::string stime = "2025-06-26 12:48:00";
	std::string etime = "2025-06-26 12:50:00";
	RunBigQueryScript(stime, etime);

	PlayBackSBSStream= new TStreamReader("D:\\bbh\\2025\\Architect\\CMU_Lecture\\new_git\\RUI\\Recorded\\bigquery_data.sbs");
	if (PlayBackSBSStream==NULL)
	  {
		ShowMessage("Cannot Open File bigquery_data.sbs");
	  }
	 else {
		   TCPClientSBSHandleThread = new TTCPClientSBSHandleThread(true);
		   TCPClientSBSHandleThread->UseFileInsteadOfNetwork=true;
		   TCPClientSBSHandleThread->First=true;
		   TCPClientSBSHandleThread->FreeOnTerminate=TRUE;
		   TCPClientSBSHandleThread->Resume();
		   SBSPlaybackButton->Caption="Stop SBS Playback";
           SBSConnectButton->Enabled=false;
		  }
	}
  else
  {
   TCPClientSBSHandleThread->Terminate();
   delete PlayBackSBSStream;
   PlayBackSBSStream=NULL;
   SBSPlaybackButton->Caption="SBS Playback";
   SBSConnectButton->Enabled=true;
  }
*/
  if ((SBSPlaybackButton->Caption=="SBS Playback") && (Sender!=NULL))
 {
  if (PlaybackSBSDialog->Execute())
   {
	// First, check if the file exists.
	if (!FileExists(PlaybackSBSDialog->FileName))
	  ShowMessage("File "+PlaybackSBSDialog->FileName+" does not exist");
	else
	{
		// Open a file for writing. Creates the file if it doesn't exist, or overwrites it if it does.
	PlayBackSBSStream= new TStreamReader(PlaybackSBSDialog->FileName);
	if (PlayBackSBSStream==NULL)
	  {
		ShowMessage("Cannot Open File "+PlaybackSBSDialog->FileName);
	  }
	 else {
		   TCPClientSBSHandleThread = new TTCPClientSBSHandleThread(true);
		   TCPClientSBSHandleThread->UseFileInsteadOfNetwork=true;
		   TCPClientSBSHandleThread->First=true;
		   TCPClientSBSHandleThread->FreeOnTerminate=TRUE;
		   TCPClientSBSHandleThread->Resume();
		   SBSPlaybackButton->Caption="Stop SBS Playback";
		   SBSConnectButton->Enabled=false;
		   SBSRecordButton->Enabled=false;
		   RawConnectButton->Enabled=false;
		   RawPlaybackButton->Enabled=false;
		   RawRecordButton->Enabled=false;
		   BigQueryPlayback->Enabled=false;

		  }
	}
  }
 }
 else
 {
   TCPClientSBSHandleThread->Terminate();
   delete PlayBackSBSStream;
   PlayBackSBSStream=NULL;
   SBSPlaybackButton->Caption="SBS Playback";

   SBSConnectButton->Enabled=true;
   SBSPlaybackButton->Enabled=true;
   SBSRecordButton->Enabled=true;
   RawConnectButton->Enabled=true;
   RawPlaybackButton->Enabled=true;
   RawRecordButton->Enabled=true;
   BigQueryPlayback->Enabled=true;

 }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::IdTCPClientSBSConnected(TObject *Sender)
{
   //SetKeepAliveValues(const AEnabled: Boolean; const ATimeMS, AInterval: Integer);
   IdTCPClientSBS->Socket->Binding->SetKeepAliveValues(true,60*1000,15*1000);
   SBSConnectButton->Caption="SBS Disconnect";
   
   SBSPlaybackButton->Enabled=false;
   
   RawConnectButton->Enabled=false;
   RawPlaybackButton->Enabled=false;
   RawRecordButton->Enabled=false;

   BigQueryPlayback->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::IdTCPClientSBSDisconnected(TObject *Sender)
{
  TCPClientSBSHandleThread->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TimeToGoTrackBarChange(TObject *Sender)
{
  _int64 hmsm;
  hmsm=TimeToGoTrackBar->Position*1000;
  TimeToGoText->Caption=TimeToChar(hmsm);
}
//---------------------------------------------------------------------------
#ifndef YAKI_TEST_CODE
void __fastcall TForm1::LoadAirport() {
    DWORD dwFlags;
    if (!InternetGetConnectedState(&dwFlags, 0)) {
        printf("Check internet connection\n");
        return;
    }

    try {
        TIdHTTP *http = new TIdHTTP(NULL);
        TIdSSLIOHandlerSocketOpenSSL *ssl = new TIdSSLIOHandlerSocketOpenSSL(NULL);
        ssl->SSLOptions->Method = sslvTLSv1_2;         // TLS 1.2 ���
        ssl->SSLOptions->Mode = sslmClient;            // Ŭ���̾�Ʈ ���
        http->IOHandler = ssl;
        TFileStream *fileStream = new TFileStream(AirportDBPathFileName, fmCreate);

        try {
            http->Get(AIRPORT_DATABASE_URL, fileStream);
            // Airport Load
        }
        catch (const Exception &e) {
        }

        delete fileStream;
        delete http;
        delete ssl;
    }
    catch (const Exception &e) {
    }
}
void __fastcall TForm1::LoadRoute() {
    DWORD dwFlags;
    if (!InternetGetConnectedState(&dwFlags, 0)) {
        printf("Check internet connection\n");
        return;
    }
    try {
        TIdHTTP *http = new TIdHTTP(NULL);
        TIdSSLIOHandlerSocketOpenSSL *ssl = new TIdSSLIOHandlerSocketOpenSSL(NULL);
        ssl->SSLOptions->Method = sslvTLSv1_2;         // TLS 1.2 ���
        ssl->SSLOptions->Mode = sslmClient;            // Ŭ���̾�Ʈ ���
        http->IOHandler = ssl;
        TFileStream *fileStream = new TFileStream(RouteDBPathFileName, fmCreate);

        try {
            http->Get(ROUTE_DATABASE_URL, fileStream);
            // Airport Load
        }
        catch (const Exception &e) {
        }

        delete fileStream;
        delete http;
        delete ssl;
    }
    catch (const Exception &e) {
    }
}
#endif
//---------------------------------------------------------------------------

void __fastcall TForm1::LoadMap(int Type)
{
    AnsiString baseDir = ExtractFilePath(ExtractFileDir(Application->ExeName));
    IMapProvider* provider = MapFactory::Instance().Create(Type);
    if (!provider) {
        ShowMessage("Invalid map type");
        return;
    }

    provider->Initialize(LoadMapFromInternet, baseDir);
    g_GETileManager = provider->GetTileManager();
    g_MasterLayer = provider->CreateLayer();
    g_EarthView = new FlatEarthView(g_MasterLayer);
    g_EarthView->Resize(ObjectDisplay->Width, ObjectDisplay->Height);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MapComboBoxChange(TObject *Sender)
{
  double    m_Eyeh= g_EarthView->m_Eye.h;
  double    m_Eyex= g_EarthView->m_Eye.x;
  double    m_Eyey= g_EarthView->m_Eye.y;


  Timer1->Enabled=false;
  Timer2->Enabled=false;
  delete g_EarthView;
  if (g_GETileManager) delete g_GETileManager;
  delete g_MasterLayer;
   if (g_Keyhole) {
     g_Keyhole->SetSaveStorage(nullptr);
   }
  delete g_Storage;
  if (LoadMapFromInternet)
  {
   if (g_Keyhole) delete g_Keyhole;
   g_Keyhole = nullptr;
  }
  DWORD dwFlags;

  if (MapComboBox->ItemIndex==0) {
#ifndef YAKI_TEST_CODE
  if (!InternetGetConnectedState(&dwFlags, 0)) {
    LoadMapFromInternet=false;
  } else {
    LoadMapFromInternet=true;
  }
#else
    LoadMapFromInternet = true;
#endif
    LoadMap(GoogleMaps);
  }
  else if (MapComboBox->ItemIndex==1) {
   LoadMapFromInternet = false;
   LoadMap(SkyVector_VFR);
  }
  else if (MapComboBox->ItemIndex==2) {
    LoadMapFromInternet = false;
    LoadMap(SkyVector_IFR_Low);
  }
  else if (MapComboBox->ItemIndex==3) {
    LoadMapFromInternet = false;
    LoadMap(SkyVector_IFR_High);
   }
  else if (MapComboBox->ItemIndex==4) {
#ifndef YAKI_TEST_CODE
  if (!InternetGetConnectedState(&dwFlags, 0)) {
    LoadMapFromInternet=false;
  } else {
    LoadMapFromInternet=true;
  }
#else
    LoadMapFromInternet = true;
#endif
    LoadMap(OpenStreetMap);
  }
   g_EarthView->m_Eye.h =m_Eyeh;
   g_EarthView->m_Eye.x=m_Eyex;
   g_EarthView->m_Eye.y=m_Eyey;
   Timer1->Enabled=true;
   Timer2->Enabled=true;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::BigQueryCheckBoxClick(TObject *Sender)
{
 if (BigQueryCheckBox->State==cbChecked) CreateBigQueryCSV();
 else {
	   CloseBigQueryCSV();
	   RunPythonScript(BigQueryPythonScript,BigQueryPath+" "+BigQueryCSVFileName);
	  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CreateBigQueryCSV(void)
{
    AnsiString  HomeDir = ExtractFilePath(ExtractFileDir(Application->ExeName));
    BigQueryCSVFileName="BigQuery"+UIntToStr(BigQueryFileCount)+".csv";
    BigQueryRowCount=0;
    BigQueryFileCount++;
    BigQueryCSV=new TStreamWriter(HomeDir+"..\\BigQuery\\"+BigQueryCSVFileName, false);
    if (BigQueryCSV==NULL)
	  {
		ShowMessage("Cannot Open BigQuery CSV File "+HomeDir+"..\\BigQuery\\"+BigQueryCSVFileName);
        BigQueryCheckBox->State=cbUnchecked;
	  }
	AnsiString Header=AnsiString("Message Type,Transmission Type,SessionID,AircraftID,HexIdent,FlightID,Date_MSG_Generated,Time_MSG_Generated,Date_MSG_Logged,Time_MSG_Logged,Callsign,Altitude,GroundSpeed,Track,Latitude,Longitude,VerticalRate,Squawk,Alert,Emergency,SPI,IsOnGround");
	BigQueryCSV->WriteLine(Header);
}
//--------------------------------------------------------------------------
void __fastcall TForm1::CloseBigQueryCSV(void)
{
    if (BigQueryCSV)
    {
     delete BigQueryCSV;
     BigQueryCSV=NULL;
    }
}
//--------------------------------------------------------------------------
	 static void RunPythonScript(AnsiString scriptPath,AnsiString args)
     {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        AnsiString commandLine = "python " + scriptPath+" "+args;
        char* cmdLineCharArray = new char[strlen(commandLine.c_str()) + 1];
		strcpy(cmdLineCharArray, commandLine.c_str());
	#define  LOG_PYTHON 1
	#if LOG_PYTHON
        //printf("%s\n", cmdLineCharArray);
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
	    sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;
		HANDLE h = CreateFileA(Form1->BigQueryLogFileName.c_str(),
		FILE_APPEND_DATA,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &sa,
		OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
		NULL );

        si.hStdInput = NULL;
	    si.hStdOutput = h;
	    si.hStdError = h; // Redirect standard error as well, if needed
	    si.dwFlags |= STARTF_USESTDHANDLES;
    #endif
        if (!CreateProcessA(
            nullptr,          // No module name (use command line)
            cmdLineCharArray, // Command line
            nullptr,          // Process handle not inheritable
            nullptr,          // Thread handle not inheritable
	 #if LOG_PYTHON
            TRUE,
     #else
            FALSE,            // Set handle inheritance to FALSE
     #endif
            CREATE_NO_WINDOW, // Don't create a console window
			nullptr,          // Use parent's environment block
            nullptr,          // Use parent's starting directory
            &si,             // Pointer to STARTUPINFO structure
            &pi))             // Pointer to PROCESS_INFORMATION structure
         {
            std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
            delete[] cmdLineCharArray;
            return;
         }

        // Optionally, detach from the process
        CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		delete[] cmdLineCharArray;
    }

 //--------------------------------------------------------------------------
void __fastcall TForm1::UseSBSRemoteClick(TObject *Sender)
{
 SBSIpAddress->Text="data.adsbhub.org";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::UseSBSLocalClick(TObject *Sender)
{
 SBSIpAddress->Text="128.237.96.41";
}
//---------------------------------------------------------------------------
static bool DeleteFilesWithExtension(AnsiString dirPath, AnsiString extension)
 {
	AnsiString searchPattern = dirPath + "\\*." + extension;
	WIN32_FIND_DATAA findData;

	HANDLE hFind = FindFirstFileA(searchPattern.c_str(), &findData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return false; // No files found or error
	}

	do {
		AnsiString filePath = dirPath + "\\" + findData.cFileName;
		if (DeleteFileA(filePath.c_str()) == 0) {
			FindClose(hFind);
			return false; // Failed to delete a file
		}
	} while (FindNextFileA(hFind, &findData) != 0);

	FindClose(hFind);
	return true;
}
static bool IsFirstRow=true;
static bool CallBackInit=false;
//---------------------------------------------------------------------------
 static int CSV_callback_ARTCCBoundaries (struct CSV_context *ctx, const char *value)
{
  int    rc = 1;
  static char LastArea[512];
  static char Area[512];
  static char Lat[512];
  static char Lon[512];
  int    Deg,Min,Sec,Hsec;
  char   Dir;

   if (ctx->field_num==0)
   {
	strcpy(Area,value);
   }
   else if (ctx->field_num==3)
   {
	strcpy(Lat,value);
   }
   else if (ctx->field_num==4)
   {
    strcpy(Lon,value);
   }

   if (ctx->field_num == (ctx->num_fields - 1))
   {

	float fLat, fLon;
   if (!IsFirstRow)
   {
	 if (!CallBackInit)
	 {
	  strcpy(LastArea,Area);
	  CallBackInit=true;
	 }
	   if(strcmp(LastArea,Area)!=0)
		{

		 if (FinshARTCCBoundary())
		   {
			printf("Load ERROR ID %s\n",LastArea);
		   }
		 else printf("Loaded ID %s\n",LastArea);
		 strcpy(LastArea,Area);
		 }
	   if (Form1->AreaTemp==NULL)
		   {
			Form1->AreaTemp= new TArea;
			Form1->AreaTemp->NumPoints=0;
			Form1->AreaTemp->Name=Area;
			Form1->AreaTemp->Selected=false;
			Form1->AreaTemp->Triangles=NULL;
			 printf("Loading ID %s\n",Area);
		   }
	   if (sscanf(Lat,"%2d%2d%2d%2d%c",&Deg,&Min,&Sec,&Hsec,&Dir)!=5)
		 printf("Latitude Parse Error\n");
	   fLat=Deg+Min/60.0+Sec/3600.0+Hsec/360000.00;
	   if (Dir=='S') fLat=-fLat;

	   if (sscanf(Lon,"%3d%2d%2d%2d%c",&Deg,&Min,&Sec,&Hsec,&Dir)!=5)
		 printf("Longitude Parse Error\n");
	   fLon=Deg+Min/60.0+Sec/3600.0+Hsec/360000.00;
	   if (Dir=='W') fLon=-fLon;
	   //printf("%f, %f\n",fLat,fLon);
	   if (Form1->AreaTemp->NumPoints<MAX_AREA_POINTS)
	   {
		Form1->AreaTemp->Points[Form1->AreaTemp->NumPoints][1]=fLat;
		Form1->AreaTemp->Points[Form1->AreaTemp->NumPoints][0]=fLon;
		Form1->AreaTemp->Points[Form1->AreaTemp->NumPoints][2]=0.0;
		Form1->AreaTemp->NumPoints++;
	   }
		else printf("Max Area Points Reached\n");

   }
   if (IsFirstRow) IsFirstRow=false;
   }
  return(rc);
}
//---------------------------------------------------------------------------
bool __fastcall TForm1::LoadARTCCBoundaries(AnsiString FileName)
{
  CSV_context  csv_ctx;
   memset (&csv_ctx, 0, sizeof(csv_ctx));
   csv_ctx.file_name = FileName.c_str();
   csv_ctx.delimiter = ',';
   csv_ctx.callback  = CSV_callback_ARTCCBoundaries;
   csv_ctx.line_size = 2000;
   IsFirstRow=true;
   CallBackInit=false;
   if (!CSV_open_and_parse_file(&csv_ctx))
    {
	  printf("Parsing of \"%s\" failed: %s\n", FileName.c_str(), strerror(errno));
      return (false);
	}
   if ((Form1->AreaTemp!=NULL) && (Form1->AreaTemp->NumPoints>0))
   {
     char Area[512];
     strcpy(Area,Form1->AreaTemp->Name.c_str());
     if (FinshARTCCBoundary())
	    {
        printf("Loaded ERROR ID %s\n",Area);
	    }
        else printf("Loaded ID %s\n",Area);
   }
   printf("Done\n");
   return(true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::LoadARTCCBoundaries1Click(TObject *Sender)
{
   LoadARTCCBoundaries(ARTCCBoundaryDataPathFileName);
}
//---------------------------------------------------------------------------
static int FinshARTCCBoundary(void)
{
  int or1=orientation2D_Polygon( Form1->AreaTemp->Points,Form1->AreaTemp->NumPoints);
  if (or1==0)
   {
	TArea *Temp;
	Temp= Form1->AreaTemp;
	Form1->AreaTemp=NULL;
	delete  Temp;
	printf("Degenerate Polygon\n");
	return(-1);
   }
  if (or1==CLOCKWISE)
  {
	DWORD i;

	memcpy(Form1->AreaTemp->PointsAdj,Form1->AreaTemp->Points,sizeof(Form1->AreaTemp->Points));
	for (i = 0; i <Form1->AreaTemp->NumPoints; i++)
	 {
	   memcpy(Form1->AreaTemp->Points[i],
			 Form1->AreaTemp->PointsAdj[Form1->AreaTemp->NumPoints-1-i],sizeof( pfVec3));
	 }
  }
  if (checkComplex( Form1->AreaTemp->Points,Form1->AreaTemp->NumPoints))
   {
	TArea *Temp;
	Temp= Form1->AreaTemp;
	Form1->AreaTemp=NULL;
	delete  Temp;
	printf("Polygon is Complex\n");
    return(-2);
   }
  DWORD Row,Count,i;


 Count=Form1->Areas->Count;
 for (i = 0; i < Count; i++)
 {
  TArea *Area = (TArea *)Form1->Areas->Items[i];
  if (Area->Name==Form1->AreaTemp->Name) {

   TArea *Temp;
   Temp= Form1->AreaTemp;
   printf("Duplicate Area Name %s\n",Form1->AreaTemp->Name.c_str());;
   Form1->AreaTemp=NULL;
   delete  Temp;
   return(-3);
   }
 }

 triangulatePoly(Form1->AreaTemp->Points,Form1->AreaTemp->NumPoints,
				 &Form1->AreaTemp->Triangles);

 Form1->AreaTemp->Color=TColor(PopularColors[CurrentColor]);
 CurrentColor++ ;
 CurrentColor=CurrentColor%NumColors;
 Form1->Areas->Add(Form1->AreaTemp);
 Form1->AreaListView->Items->BeginUpdate();
 Form1->AreaListView->Items->Add();
 Row=Form1->AreaListView->Items->Count-1;
 Form1->AreaListView->Items->Item[Row]->Caption=Form1->AreaTemp->Name;
 Form1->AreaListView->Items->Item[Row]->Data=Form1->AreaTemp;
 Form1->AreaListView->Items->Item[Row]->SubItems->Add("");
 Form1->AreaListView->Items->EndUpdate();
 Form1->AreaTemp=NULL;
 return 0 ;
}
//---------------------------------------------------------------------------
#ifndef YAKI_TEST_CODE // MAP
void __fastcall TForm1::RegisterMapProviders() {
    MapFactory::Instance().Register(GoogleMaps, [](){ return new GoogleMapProvider(); });
    MapFactory::Instance().Register(SkyVector_VFR, [](){ return new SkyVectorMapProvider(SkyVector_VFR, "..\\VFR_Map"); });
    MapFactory::Instance().Register(SkyVector_IFR_Low, [](){ return new SkyVectorMapProvider(SkyVector_IFR_Low, "..\\IFR_Low_Map"); });
    MapFactory::Instance().Register(SkyVector_IFR_High, [](){ return new SkyVectorMapProvider(SkyVector_IFR_High, "..\\IFR_High_Map"); });
    MapFactory::Instance().Register(OpenStreetMap, [](){ return new OpenStreetMapProvider(OpenStreetMap, "..\\OpenStreetMap"); });
}
#endif

//void __fastcall TForm1::FormCreate(TObject *Sender)
//{
//    IdUDPServer1->DefaultPort = 55555;
//    IdUDPServer1->Active = true;
//}
//
//void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
//{
//    IdUDPServer1->Active = false;
//}

typedef struct {
    uint8_t sdrConnected; // 0 or 1
    uint8_t wifiEnabled;  // 0 or 1
    uint8_t reserved[62];  // Padding to make the size 64 bytes
} HeartbeatMsg_t;

void __fastcall TForm1::IdUDPServer1UDPRead(TIdUDPListenerThread *AThread, const TIdBytes AData,
          TIdSocketHandle *ABinding)
{
    int length = AData.Length;
	printf("received heartbeat: %d\n", length);

    uint8_t buf[64];
    memcpy(buf, &AData[0], 64);

    HeartbeatMsg_t *msg = (HeartbeatMsg_t *)buf;

//    if(msg->wifiEnabled) {
//        WiFiStatusText->Caption = "Connected";
//    } else {
//        WiFiStatusText->Caption = "Disconnected";
//    }

    if(msg->sdrConnected) {
        SDRStatusText->Caption = "Connected";
    } else {
        SDRStatusText->Caption = "Disconnected";
    }
}



void __fastcall TForm1::FormCreate(TObject *Sender)
{
    IdUDPServer1->DefaultPort = 55555;
	IdUDPServer1->Active = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	IdUDPServer1->Active = false;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::BigQueryPlaybackClick(TObject *Sender)
{
  
  if ((BigQueryPlayback->Caption=="BigQuery Playback") && (Sender!=NULL))
 {
	std::string stime = "2025-06-26 12:48:00";
	std::string etime = "2025-06-26 12:50:00";
	BigQueryPlayback->Caption="BigQuery Loading ...";
	BigQueryPlayback->Font->Color = clRed;
	BigQueryPlayback->Update();

	TThread::CreateAnonymousThread([this, stime, etime]() {
		RunBigQueryScript(stime, etime);  // 실제 작업

		// 작업 완료 후 UI 업데이트
		TThread::Synchronize(nullptr, [this]() {
			BigQueryPlayback->Caption = "BigQuery Complete!";
			BigQueryPlayback->Font->Color = clGreen;

			PlayBackSBSStream= new TStreamReader("D:\\bbh\\2025\\Architect\\CMU_Lecture\\new_git\\RUI\\Recorded\\bigquery_data.sbs");
			if (PlayBackSBSStream==NULL)
			{
				ShowMessage("Cannot Open File bigquery_data.sbs");
			}
			else
			{
			   TCPClientSBSHandleThread = new TTCPClientSBSHandleThread(true);
			   TCPClientSBSHandleThread->UseFileInsteadOfNetwork=true;
			   TCPClientSBSHandleThread->First=true;
			   TCPClientSBSHandleThread->FreeOnTerminate=TRUE;
			   TCPClientSBSHandleThread->Resume();
			   BigQueryPlayback->Caption="Stop BigQuery Playback";
			   SBSConnectButton->Enabled=false;
			   SBSPlaybackButton->Enabled=false;
			   SBSRecordButton->Enabled=false;
			   RawConnectButton->Enabled=false;
			   RawPlaybackButton->Enabled=false;
			   RawRecordButton->Enabled=false;

			}

		});
	})->Start();

 }
 else
 {
	TCPClientSBSHandleThread->Terminate();
	delete PlayBackSBSStream;
	PlayBackSBSStream=NULL;
	BigQueryPlayback->Caption="BigQuery Playback";
	SBSConnectButton->Enabled=true;
	SBSPlaybackButton->Enabled=true;
	SBSRecordButton->Enabled=true;
	RawConnectButton->Enabled=true;
	RawPlaybackButton->Enabled=true;
    RawRecordButton->Enabled=true;
 }

}
//---------------------------------------------------------------------------

