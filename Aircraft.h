//---------------------------------------------------------------------------

#ifndef AircraftH
#define AircraftH
#include <vector>
#include <mutex>
#include "ght_hash_table.h"
#include "DecodeRawADS_B.h"

#define MODES_NON_ICAO_ADDRESS       (1<<24) // Set on addresses to indicate they are not ICAO addresses

typedef struct
{
 uint32_t            ICAO;
 char                HexAddr[7];       /* Printable ICAO address */
 __int64             LastSeen;             /* Time at which the last packet was received. */
 long                NumMessagesRaw;      /* Number of Mode S messages received. */
 long                NumMessagesSBS;
 int                 odd_cprlat;       /* Encoded latitude and longitude as extracted by odd and even */
 int                 odd_cprlon;       /* CPR encoded messages. */
 int                 even_cprlat;
 int                 even_cprlon;
 __int64             odd_cprtime;
 __int64             even_cprtime;
 char                FlightNum[9];     /* Flight number */
 bool                HaveFlightNum;
 bool                HaveAltitude;
 double              Altitude;
 bool                HaveLatLon;
 double              Latitude;      /* Coordinated obtained from CPR encoded data. */
 double              Longitude;     /* Coordinated obtained from CPR encoded data. */
 bool                HaveSpeedAndHeading;
 double              Heading;
 double              Speed;
 double              VerticalRate;
 int                 SpriteImage;
} TADS_B_Aircraft;

struct TInsertAircraftPair {
    TADS_B_Aircraft * ADS_B_Aircraft;
    unsigned int i_key_size;
    const void *p_key_data;
};

struct TRemoveAircraftPair {
    unsigned int i_key_size;
    const void *p_key_data;
};

void RawToAircraft(modeS_message *mm,TADS_B_Aircraft *ADS_B_Aircraft);
//---------------------------------------------------------------------------
#ifndef YAKI_TEST_CODE
class AircraftManager {
public:
    AircraftManager();
    ~AircraftManager();
    static AircraftManager* GetInstance();
private:
    static AircraftManager* instance;
public:
    TList *insertAircraftList;
    TList *removeAircraftList;
    std::mutex Mtx;
public:
	ght_hash_table_t *HashTable;
    bool computeCPA;
public:
    TADS_B_Aircraft* GetAircraft(unsigned int i_key_size, const void *p_key_data);
    int Insert(TADS_B_Aircraft* ADS_B_Aircraft, unsigned int i_key_size, const void *p_key_data);
    unsigned int GetSize();
    void *GetFirst(ght_iterator_t *p_iterator, const void **pp_key);
	void *GetNext(ght_iterator_t *p_iterator, const void **pp_key);
	void *Remove(unsigned int i_key_size, const void *p_key_data);
    void MutexLock();
    void MutexUnlock();
};
#endif

// CPA 결과 한 쌍
struct TCpaPair {
    uint32_t ICAO1;
    uint32_t ICAO2;
    double a_Lat, a_Lon; // 항공기1 CPA 위치
    double b_Lat, b_Lon; // 항공기2 CPA 위치
    double Lat1, Lon1; // 항공기1 CPA 위치
    double Lat2, Lon2; // 항공기2 CPA 위치
    double Tcpa;       // 시간
    double Dist;       // CPA 3D 거리
    bool Valid;
};

// CPA Cache
class TCPAResultCache {
private:
    std::vector<TCpaPair> Cache;
    std::mutex mtx;

public:
    void Update(const std::vector<TCpaPair>& newData) {
//        std::lock_guard<std::mutex> lock(mtx);
        Cache = newData;
    }

    void Get(std::vector<TCpaPair>& outData) {
//        std::lock_guard<std::mutex> lock(mtx);
        outData = Cache;
    }
};

class TCPAWorkerThread : public TThread {
private:
    TCPAResultCache* Cache;

protected:
    void __fastcall Execute();

public:
    __fastcall TCPAWorkerThread(TCPAResultCache* cache);
};

#endif

