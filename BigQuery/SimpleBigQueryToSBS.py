'''
Make sure the following is installed:
pip install google-cloud-bigquery
pip install --upgrade google-api-python-client
'''

import os
import sys
import time
import pandas as pd
from google.cloud import bigquery
from concurrent.futures import ThreadPoolExecutor, as_completed
from datetime import datetime, timedelta

stime = time.time()
if len(sys.argv) == 4:  
   global_filepath = sys.argv[1]
   start_time_arg = sys.argv[2]
   end_time_arg = sys.argv[3]
   print(f"The first argument is: {global_filepath}")
   print(f"The start_time_arg argument is: {start_time_arg}")
   print(f"The end_time_arg argument is: {end_time_arg}")
else:
   print(f"Failure 1\n")	
   os._exit(0)
current_directory = os.getcwd()
print(current_directory)
# Set credentials
api_key = global_filepath+"YourJsonFile.json"
os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = api_key

# Construct a BigQuery client object.
client = bigquery.Client()

def make_time_ranges(start_str, end_str, step_seconds=10):
    """
    문자열 형식의 시작/끝 시간을 받아 step_seconds 간격으로 time_ranges 리스트 생성
    """
    start_time = datetime.strptime(start_str, "%Y-%m-%d %H:%M:%S")
    end_time = datetime.strptime(end_str, "%Y-%m-%d %H:%M:%S")
    
    ranges = []
    current = start_time
    while current < end_time:
        next_time = current + timedelta(seconds=step_seconds)
        if next_time > end_time:
            next_time = end_time
        ranges.append((current.strftime("%Y-%m-%d %H:%M:%S"), next_time.strftime("%Y-%m-%d %H:%M:%S")))
        current = next_time
    
    return ranges

def query_to_sbs_chunk_to_dataframe(start_time, end_time):
    time.sleep(0.5)  # 쿼리 실행 간의 짧은 지연 시간 추가
    query = f"""
    SELECT *
    FROM `scs-lg-arch-3.SBS_Data.BATMAN_SBS_Data`
    WHERE DATETIME(Date_MSG_Generated, Time_MSG_Generated) >= '{start_time}'
      AND DATETIME(Date_MSG_Generated, Time_MSG_Generated) < '{end_time}'
    """
    df = client.query(query).to_dataframe(use_bqstorage_api=True)

    print("query_to_sbs_chunk:", start_time, end_time)

    # SBS 문자열로 변환 (메모리 상에서 처리)
    sbs_lines = []
    for _, row in df.iterrows():
        sbs_lines.append(str(row["TimeStamp"]))
        other_fields = row.drop(labels="TimeStamp")
        cleaned_fields = ["" if pd.isna(val) else str(val) for val in other_fields.tolist()]
        sbs_lines.append(",".join(cleaned_fields))
    return "\n".join(sbs_lines)

def query_to_sbs_chunk(start_time, end_time):
    from google.cloud import bigquery
    import time

    time.sleep(0.5)  # 쿼리 실행 간의 짧은 지연 시간 추가

    ## 2025-06-26 12:48:00    2025-06-26 12:50:00

    

    query = f"""
    SELECT *
    FROM `scs-lg-arch-3.SBS_Data.BATMAN_SBS_Data`
    WHERE 
        # DATETIME(Date_MSG_Generated, Time_MSG_Generated) >= '{start_time}'
        # AND DATETIME(Date_MSG_Generated, Time_MSG_Generated) < '{end_time}'

        Date_MSG_Generated BETWEEN DATE '{str(start_time).split(" ")[0]}' AND DATE '{str(end_time).split(" ")[0]}'
        AND Time_MSG_Generated BETWEEN TIME '{str(start_time).split(" ")[1]}' AND TIME '{str(end_time).split(" ")[1]}'

    """
    query_job = client.query(query)
    results = query_job.result()  # row iterator

    print("query_to_sbs_chunk:", start_time, end_time)

    # SBS 문자열로 변환
    sbs_lines = []
    field_names = [field.name for field in results.schema]
    for row in results:
        row_dict = dict(zip(field_names, row))
        timestamp = str(row_dict.get("TimeStamp", ""))
        sbs_lines.append(timestamp)

        other_fields = [row_dict.get(f, "") for f in field_names if f != "TimeStamp"]
        cleaned_fields = ["" if pd.isna(val) else str(val) for val in other_fields]
        sbs_lines.append(",".join(cleaned_fields))

    return "\n".join(sbs_lines)

# 구간 설정
time_ranges = make_time_ranges(start_time_arg, end_time_arg, step_seconds=10)
print("Time taken make_time_ranges:", len(time_ranges), time.time() - stime)

# 병렬 실행
max_workers = min(len(time_ranges), 10)
#with ThreadPoolExecutor(max_workers=max_workers) as executor:
#    futures = [executor.submit(query_to_sbs_chunk, start, end) for start, end in time_ranges]

#print("Time taken query_to_sbs_chunk:", time.time() - stime)
## 모든 결과 받아와서 하나의 문자열로 병합
#all_sbs_content = "\n".join(f.result() for f in futures)

results = []
with ThreadPoolExecutor(max_workers=max_workers) as executor:
    futures = {executor.submit(query_to_sbs_chunk, start, end): (start, end) for start, end in time_ranges}
    for future in as_completed(futures):
        try:
            results.append(future.result())
        except Exception as e:
            start, end = futures[future]
            print(f"Error in time range {start} to {end}: {e}")

print("Time taken query_to_sbs_chunk:", time.time() - stime)
all_sbs_content = "\n".join(results)

print("Time taken all_sbs_content:", time.time() - stime)

# SBS 파일로 저장
#timestamp_str = datetime.now().strftime("%Y%m%d_%H%M%S")
sbs_file = global_filepath + f"..\\Recorded\\bigquery_datas.sbs"
last_sbs_file = global_filepath + f"..\\Recorded\\bigquery_data.sbs"
with open(sbs_file, 'w', encoding='utf-8') as f:
    f.write(all_sbs_content)


with open(sbs_file, "r", encoding="utf-8") as infile:
    lines = infile.readlines()
# 빈 줄 제거
cleaned_lines = [line for line in lines if line.strip() != ""]

with open(last_sbs_file, "w", encoding="utf-8") as outfile:
    outfile.writelines(cleaned_lines)

print("Time taken all_sbs_content write:", time.time() - stime)
print(f"SBS file saved: {sbs_file}")