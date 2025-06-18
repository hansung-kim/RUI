//---------------------------------------------------------------------------

#pragma hdrstop

#include "Aircraft.h"
#include "TimeFunctions.h"
#include "CPA.h"
#include "LatLonConv.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
static int cprModFunction(int a, int b);
static int cprNLFunction(double lat);
static int cprNFunction(double lat, int isodd);
static double cprDlonFunction(double lat, int isodd);
static void decodeCPR(TADS_B_Aircraft *a);

//---------------------------------------------------------------------------
/* Always positive MOD operation, used for CPR decoding. */
static int cprModFunction(int a, int b)
{
    int res = a % b;
    if (res < 0) res += b;
    return res;
}
//---------------------------------------------------------------------------
/* The NL function uses the precomputed table from 1090-WP-9-14 */
static int cprNLFunction(double lat)
{
    if (lat < 0) lat = -lat; /* Table is simmetric about the equator. */
    if (lat < 10.47047130) return 59;
    if (lat < 14.82817437) return 58;
    if (lat < 18.18626357) return 57;
	if (lat < 21.02939493) return 56;
    if (lat < 23.54504487) return 55;
    if (lat < 25.82924707) return 54;
    if (lat < 27.93898710) return 53;
    if (lat < 29.91135686) return 52;
    if (lat < 31.77209708) return 51;
    if (lat < 33.53993436) return 50;
    if (lat < 35.22899598) return 49;
    if (lat < 36.85025108) return 48;
    if (lat < 38.41241892) return 47;
    if (lat < 39.92256684) return 46;
    if (lat < 41.38651832) return 45;
    if (lat < 42.80914012) return 44;
    if (lat < 44.19454951) return 43;
    if (lat < 45.54626723) return 42;
    if (lat < 46.86733252) return 41;
    if (lat < 48.16039128) return 40;
    if (lat < 49.42776439) return 39;
    if (lat < 50.67150166) return 38;
    if (lat < 51.89342469) return 37;
    if (lat < 53.09516153) return 36;
    if (lat < 54.27817472) return 35;
    if (lat < 55.44378444) return 34;
    if (lat < 56.59318756) return 33;
    if (lat < 57.72747354) return 32;
    if (lat < 58.84763776) return 31;
    if (lat < 59.95459277) return 30;
    if (lat < 61.04917774) return 29;
    if (lat < 62.13216659) return 28;
    if (lat < 63.20427479) return 27;
    if (lat < 64.26616523) return 26;
    if (lat < 65.31845310) return 25;
    if (lat < 66.36171008) return 24;
    if (lat < 67.39646774) return 23;
    if (lat < 68.42322022) return 22;
    if (lat < 69.44242631) return 21;
    if (lat < 70.45451075) return 20;
    if (lat < 71.45986473) return 19;
    if (lat < 72.45884545) return 18;
    if (lat < 73.45177442) return 17;
    if (lat < 74.43893416) return 16;
    if (lat < 75.42056257) return 15;
    if (lat < 76.39684391) return 14;
    if (lat < 77.36789461) return 13;
    if (lat < 78.33374083) return 12;
    if (lat < 79.29428225) return 11;
    if (lat < 80.24923213) return 10;
    if (lat < 81.19801349) return 9;
    if (lat < 82.13956981) return 8;
    if (lat < 83.07199445) return 7;
    if (lat < 83.99173563) return 6;
    if (lat < 84.89166191) return 5;
    if (lat < 85.75541621) return 4;
    if (lat < 86.53536998) return 3;
    if (lat < 87.00000000) return 2;
    else return 1;
}
//---------------------------------------------------------------------------
static int cprNFunction(double lat, int isodd)
{
    int nl = cprNLFunction(lat) - isodd;
    if (nl < 1) nl = 1;
    return nl;
}
//---------------------------------------------------------------------------
static double cprDlonFunction(double lat, int isodd)
{
    return 360.0 / cprNFunction(lat, isodd);
}
//---------------------------------------------------------------------------
/* This algorithm comes from:
 * http://www.lll.lu/~edward/edward/adsb/DecodingADSBposition.html.
 *
 *
 * A few remarks:
 * 1) 131072 is 2^17 since CPR latitude and longitude are encoded in 17 bits.
 * 2) We assume that we always received the odd packet as last packet for
 *    simplicity. This may provide a position that is less fresh of a few
 *    seconds.
 */
static void decodeCPR(TADS_B_Aircraft *a)
{
    const double AirDlat0 = 360.0 / 60;
    const double AirDlat1 = 360.0 / 59;
    double lat0 = a->even_cprlat;
    double lat1 = a->odd_cprlat;
    double lon0 = a->even_cprlon;
    double lon1 = a->odd_cprlon;

    /* Compute the Latitude Index "j" */
    int j = floor(((59*lat0 - 60*lat1) / 131072) + 0.5);
    double rlat0 = AirDlat0 * (cprModFunction(j,60) + lat0 / 131072);
    double rlat1 = AirDlat1 * (cprModFunction(j,59) + lat1 / 131072);

    if (rlat0 >= 270) rlat0 -= 360;
    if (rlat1 >= 270) rlat1 -= 360;

    /* Check that both are in the same latitude zone, or abort. */
    if (cprNLFunction(rlat0) != cprNLFunction(rlat1)) return;

    /* Compute ni and the longitude index m */
    if (a->even_cprtime > a->odd_cprtime) {
        /* Use even packet. */
        int ni = cprNFunction(rlat0,0);
        int m = floor((((lon0 * (cprNLFunction(rlat0)-1)) -
                        (lon1 * cprNLFunction(rlat0))) / 131072) + 0.5);
        a->Longitude = cprDlonFunction(rlat0,0) * (cprModFunction(m,ni)+lon0/131072);
        a->Latitude = rlat0;
    } else {
        /* Use odd packet. */
        int ni = cprNFunction(rlat1,1);
		int m = floor((((lon0 * (cprNLFunction(rlat1)-1)) -
                        (lon1 * cprNLFunction(rlat1))) / 131072.0) + 0.5);
        a->Longitude = cprDlonFunction(rlat1,1) * (cprModFunction(m,ni)+lon1/131072);
        a->Latitude = rlat1;
    }
    if (a->Longitude > 180) a->Longitude -= 360;
}

 //---------------------------------------------------------------------------
 void RawToAircraft(modeS_message *mm,TADS_B_Aircraft *ADS_B_Aircraft)
 {
	  __int64 CurrentTime=GetCurrentTimeInMsec();
	 ADS_B_Aircraft->LastSeen =CurrentTime;
	 ADS_B_Aircraft->NumMessagesRaw++;

	if (mm->msg_type == 0 || mm->msg_type == 4 || mm->msg_type == 20)
	  {
		ADS_B_Aircraft->Altitude = mm->altitude;
		ADS_B_Aircraft->HaveAltitude=true;
	  }
	else if (mm->msg_type == 17)
	  {
		if (mm->ME_type >= 1 && mm->ME_type <= 4)
		{
			memcpy(ADS_B_Aircraft->FlightNum, mm->flight, sizeof(ADS_B_Aircraft->FlightNum));
			ADS_B_Aircraft->HaveFlightNum=true;
		}
		else if (mm->ME_type >= 9 && mm->ME_type <= 18)
		{
			ADS_B_Aircraft->Altitude = mm->altitude;
			ADS_B_Aircraft->HaveAltitude=true;
			if (mm->odd_flag)
			  {
				ADS_B_Aircraft->odd_cprlat = mm->raw_latitude;
				ADS_B_Aircraft->odd_cprlon = mm->raw_longitude;
				ADS_B_Aircraft->odd_cprtime = CurrentTime;
              }
			 else
             {
				ADS_B_Aircraft->even_cprlat = mm->raw_latitude;
                ADS_B_Aircraft->even_cprlon = mm->raw_longitude;
				ADS_B_Aircraft->even_cprtime =CurrentTime;
             }
			/* If the two data is less than 10 seconds apart, compute
             * the position. */
			if (llabs(ADS_B_Aircraft->even_cprtime - ADS_B_Aircraft->odd_cprtime) <= 10000)
            {
				decodeCPR(ADS_B_Aircraft);
				ADS_B_Aircraft->HaveLatLon=true;
			}
		}
		else if (mm->ME_type == 19) {
			if (mm->ME_subtype== 1 || mm->ME_subtype == 2)
			{
				ADS_B_Aircraft->Speed = mm->velocity;
				ADS_B_Aircraft->Heading = mm->heading;
                ADS_B_Aircraft->VerticalRate=(mm->vert_rate_sign==0?1:-1) * (mm->vert_rate-1) * 64;
				ADS_B_Aircraft->HaveSpeedAndHeading=true;
			}
		}
	}

 }
  //---------------------------------------------------------------------------
  AircraftManager* AircraftManager::instance = nullptr;

  AircraftManager::AircraftManager() {
    HashTable = ght_create(50000);
    if ( !HashTable)
    {
        throw Sysutils::Exception("Create Hash Failed");
    }
    ght_set_rehash(HashTable, TRUE);
    insertAircraftList = new TList;
    removeAircraftList = new TList;
    computeCPA = false;

}
AircraftManager::~AircraftManager() {
    ght_finalize(HashTable);
    if (insertAircraftList) {
         delete insertAircraftList;
    }
    if (removeAircraftList) {
         delete removeAircraftList;
    }
}

AircraftManager* AircraftManager::GetInstance() {
    if (instance == nullptr) {
        instance = new AircraftManager();
    }
    return instance;
}

TADS_B_Aircraft* AircraftManager::GetAircraft(unsigned int i_key_size, const void *p_key_data) {
	return (TADS_B_Aircraft *)ght_get(HashTable, i_key_size, p_key_data);
}

int AircraftManager::Insert(TADS_B_Aircraft* ADS_B_Aircraft, unsigned int i_key_size, const void *p_key_data) {
    if (computeCPA) {
        TInsertAircraftPair *insertAircraft = new TInsertAircraftPair;
        insertAircraft->ADS_B_Aircraft = ADS_B_Aircraft;
        insertAircraft->i_key_size = i_key_size;
        insertAircraft->p_key_data = p_key_data;
        insertAircraftList->Add(insertAircraft);
        return 0;
    }
    return ght_insert(HashTable, ADS_B_Aircraft, i_key_size, p_key_data);
}

unsigned int AircraftManager::GetSize() {
    return ght_size(HashTable);
}

void *AircraftManager::GetFirst(ght_iterator_t *p_iterator, const void **pp_key) {
    return ght_first(HashTable, p_iterator, pp_key);
}

void *AircraftManager::GetNext(ght_iterator_t *p_iterator, const void **pp_key) {
    return ght_next(HashTable, p_iterator, pp_key);
}

void *AircraftManager::Remove(unsigned int i_key_size, const void *p_key_data) {
    if (computeCPA) {
        TRemoveAircraftPair *removeAircraft = new TRemoveAircraftPair;
        removeAircraft->i_key_size = i_key_size;
        removeAircraft->p_key_data = p_key_data;
        removeAircraftList->Add(removeAircraft);
        return (void*)p_key_data; // YAKI_TEST_CODE
    }
    return ght_remove(HashTable, i_key_size, p_key_data);
}

void AircraftManager::MutexLock() {
    Mtx.lock();
}
void AircraftManager::MutexUnlock() {
    Mtx.unlock();
}

constexpr double KM_TO_NM = 0.539957;
constexpr double FEET_TO_KM = 0.0003048;

__fastcall TCPAWorkerThread::TCPAWorkerThread(TCPAResultCache* cache)
    : TThread(true), Cache(cache)
{
    FreeOnTerminate = false;
}
#ifndef YAKI_TEST_CODE
 // deg -> radian 변환 매크로
constexpr double DEG2RAD = M_PI / 180.0;

// 결과: km 단위 거리 반환
static double Haversine(double lat1_deg, double lon1_deg, double lat2_deg, double lon2_deg)
{
    const double R = 6371.0; // 지구 반경 (km)

    // 위도, 경도를 radian으로 변환
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

// Nautical Miles(NM)로 바로 얻고 싶으면:
static double HaversineNM(double lat1, double lon1, double lat2, double lon2)
{
    const double KM_TO_NM = 0.539957;
    return Haversine(lat1, lon1, lat2, lon2) * KM_TO_NM;
}
#endif
void __fastcall TCPAWorkerThread::Execute() {
    while (!Terminated) {
        std::vector<TCpaPair> NewCache;

        auto mgr = AircraftManager::GetInstance();
        ght_iterator_t it1, it2;
        uint32_t* key1;
        uint32_t* key2;
//        mgr->MutexLock();
        mgr->computeCPA = true;
        TADS_B_Aircraft* a1 = (TADS_B_Aircraft*)mgr->GetFirst(&it1, (const void**)&key1);
        while (a1) {

            if (!a1->HaveLatLon) {
            	a1 = (TADS_B_Aircraft*)mgr->GetNext(&it1, (const void**)&key1);
            	continue;
            }

            it2 = it1;
            TADS_B_Aircraft* a2 = (TADS_B_Aircraft*)mgr->GetNext(&it2, (const void**)&key2);
            while (a2) {
                if (!a2->HaveLatLon) {
                    a2 = (TADS_B_Aircraft*)mgr->GetNext(&it2, (const void**)&key2);
                	continue;
                }
#if 1
                double h = HaversineNM(a1->Latitude, a1->Longitude, a2->Latitude, a2->Longitude);
                if (h >= 30.0) {
                    a2 = (TADS_B_Aircraft*)mgr->GetNext(&it2, (const void**)&key2);
                	continue;
                }
#endif
                double tcpa, cpa_distance, v_cpa;
                if (computeCPA(
                        a1->Latitude, a1->Longitude, a1->Altitude,
                        a1->Speed, a1->Heading,
                        a2->Latitude, a2->Longitude, a2->Altitude,
                        a2->Speed, a2->Heading,
                        tcpa, cpa_distance, v_cpa)) {

                    if (tcpa >= 300.0) {
                    	a2 = (TADS_B_Aircraft*)mgr->GetNext(&it2, (const void**)&key2);
                        continue;
                    }

                    v_cpa *= FEET_TO_KM * KM_TO_NM;
                    double dist = std::sqrt(cpa_distance * cpa_distance + v_cpa * v_cpa);
#if 1
                    if (std::isnan(dist) || dist >= 1.0) {
                        a2 = (TADS_B_Aircraft*)mgr->GetNext(&it2, (const void**)&key2);
                        continue;
                    }
#endif
                    double lat1, lon1, lat2, lon2, junk;
                    if (VDirect(a1->Latitude, a1->Longitude, a1->Heading, a1->Speed / 3600.0 * tcpa, &lat1, &lon1, &junk) != OKNOERROR) {
						a2 = (TADS_B_Aircraft*)mgr->GetNext(&it2, (const void**)&key2);
                    	continue;
                    }
                    if (VDirect(a2->Latitude, a2->Longitude, a2->Heading, a2->Speed / 3600.0 * tcpa, &lat2, &lon2, &junk) != OKNOERROR) {
						a2 = (TADS_B_Aircraft*)mgr->GetNext(&it2, (const void**)&key2);
                        continue;
                    }
                    TCpaPair pair;
                    pair.ICAO1 = a1->ICAO;
                    pair.ICAO2 = a2->ICAO;
                    pair.a_Lat = a1->Latitude;
                    pair.a_Lon = a1->Longitude;
                    pair.b_Lat = a2->Latitude;
                    pair.b_Lon = a2->Longitude;
                    pair.Lat1 = lat1;
                    pair.Lon1 = lon1;
                    pair.Lat2 = lat2;
                    pair.Lon2 = lon2;
                    pair.Tcpa = tcpa;
                    pair.Dist = dist;
                    pair.Valid = true;

                    NewCache.push_back(pair);
                }
                a2 = (TADS_B_Aircraft*)mgr->GetNext(&it2, (const void**)&key2);
            }
            a1 = (TADS_B_Aircraft*)mgr->GetNext(&it1, (const void**)&key1);
        }
//        mgr->MutexUnlock();
        mgr->computeCPA = false;
        // 남은 객체 출력
        for (int i = 0; i < mgr->insertAircraftList->Count; i++) {
            TInsertAircraftPair *obj = (TInsertAircraftPair*)mgr->insertAircraftList->Items[i];
            mgr->Insert(obj->ADS_B_Aircraft, obj->i_key_size, obj->p_key_data);
        }

        // 메모리 해제
        for (int i = 0; i < mgr->insertAircraftList->Count; i++) {
            delete (TInsertAircraftPair*)(mgr->insertAircraftList->Items[i]);
        }
        mgr->insertAircraftList->Clear();
        // 남은 객체 출력
        for (int i = 0; i < mgr->removeAircraftList->Count; i++) {
            TInsertAircraftPair *obj = (TInsertAircraftPair*)mgr->removeAircraftList->Items[i];
            mgr->Remove(obj->i_key_size, obj->p_key_data);
        }

        // 메모리 해제
        for (int i = 0; i < mgr->removeAircraftList->Count; i++) {
            delete (TInsertAircraftPair*)(mgr->removeAircraftList->Items[i]);
        }
        mgr->removeAircraftList->Clear();
        printf("Update cache\n");
        Cache->Update(NewCache);
        TThread::Sleep(500); // 0.5초마다 갱신
    }
}

