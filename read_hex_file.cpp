#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <cstdint>
#include <ctime>
#include <math.h>
#include <iomanip>
#include <ctime>
using namespace std;

ifstream file;
const int32_t epoch_diff  = 2082844800;
const size_t nbytes = 4;
uint64_t boxSize = 0;
vector<char> buff; int timeScale;

void readMvhd(); // movie header box
void readTrak(); // track box
void readTkhd(); // track header box
void readMdia(); // media box
void readedts(); // edit box
void readelst(); // edit list box
void readMdhd(); // media header box
void readHdlr(); // Handler Reference box
void readMinf(); // Media information box
void readDinf(); // Data Information Box
void readDref(); // Data Reference Box
void readStbl(); //	Sample Table Box
void readStdt(); // Sample Description Box
void readStts(); // Time to Sample Box

uint64_t calSize(size_t n)
{
    buff.resize(n);
    uint64_t currentSize = 0;
    if (file.read(buff.data(), buff.size())) {
        const auto nread = file.gcount();
        vector<uint8_t> bytes(buff.begin(), buff.begin() + nread);

        for (size_t i = 0; i < bytes.size(); i++) {
            currentSize += uint64_t(bytes[i] * pow(16, 2 * (bytes.size() - 1 - i)));
        }
    }

    return currentSize;
}

void print(size_t n) {
    buff.resize(n);
    if (file.read(buff.data(), buff.size())) {
        const auto nread = file.gcount();
        vector<uint8_t> bytes(buff.begin(), buff.begin() + nread);

        for (size_t i = 0; i < bytes.size(); i++){
            printf("%c", char(bytes[i]));
        }
    }
}

void skipData(size_t n){
    buff.resize(n);
    if (file.read(buff.data(), buff.size())) {
        const auto nread = file.gcount();
        vector<uint8_t> bytes(buff.begin(), buff.begin() + nread);

        for (size_t i = 0; i < bytes.size(); i++){}
    }
}

bool printV(size_t n){
	buff.resize(n);
	bool flag = true;
    if (file.read(buff.data(), buff.size())) {
        const auto nread = file.gcount();
        vector<uint8_t> bytes(buff.begin(), buff.begin() + nread);

		for (size_t i = 0; i < bytes.size(); i++){
            if(char(bytes[i]) != '\0') flag = false;
        }

        if(flag) cout << "0";
		else cout << "1";
    }

	return flag;
}

void formatDateTime(std::int64_t seconds_since_1904) {
    std::tm* timeinfo = std::gmtime(&(seconds_since_1904 -= epoch_diff));
    cout << timeinfo->tm_mday << "/" << (timeinfo->tm_mon + 1) << "/" << (timeinfo->tm_year + 1900)
    << " " << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec;
}

void convertSecondsToHMS(int seconds) {
    int hours = seconds / 3600;
    int remainder = seconds % 3600;
    int minutes = remainder / 60;
    int secs = remainder % 60;

    cout << hours << ":" << minutes << ":" << secs;
}

void calTime(size_t n){
	buff.resize(n);
    uint64_t creationTime = 0;
    if (file.read(buff.data(), buff.size())) {
        const auto nread = file.gcount();
        vector<uint8_t> bytes(buff.begin(), buff.begin() + nread);

        for (size_t i = 0; i < bytes.size(); i++) {
            creationTime += uint64_t(bytes[i] * pow(16, 2 * (bytes.size() - 1 - i)));
        }
    }

	formatDateTime(creationTime);
}

void readFtypBox()
{
    cout << "Ftyp Box:";
    boxSize = calSize(nbytes);
    cout << "\n\tSize of box: " << boxSize << " bytes";	 boxSize -= nbytes;

    cout << "\n\tName: ";	
    print(nbytes);				boxSize -= nbytes;

    cout << "\n\tMajor_brand: ";
    print(nbytes);				boxSize -= nbytes;

    cout << "\n\tMinor_version: ";
    print(nbytes);				boxSize -= nbytes;

    cout << "\n\tCompatible_brands: ";
    while (boxSize > 0) {
        print(nbytes);
        boxSize -= nbytes;
        if (boxSize >= 1) cout << ", ";
    }
}

void readMoovBox()
{
    cout << "\nMoov Box:";
    boxSize = calSize(nbytes);
    cout << "\n\tSize of box: " << boxSize + nbytes;	boxSize -= nbytes;
	cout << "\n\tName: ";	print(nbytes);				boxSize -= nbytes;
	readMvhd();
	readTrak();
}

void readMvhd(){
	bool version;
	cout << "\n\tMovie header box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\tName: ";	print(nbytes);								boxSize -= nbytes;
	cout << "\n\t\tVersion: "; version = printV(nbytes/4);				boxSize -= nbytes/4;
	cout << "\n\t\tFlags: "; print(3*nbytes/4);							boxSize -= 3*nbytes/4;

	size_t newBytes = nbytes;
	if(!version){ newBytes *= 2; }

	cout << "\n\t\tCreate_time: ";	calTime(newBytes);					boxSize -= newBytes;
	cout << "\n\t\tModification_time: "; calTime(newBytes);				boxSize -= newBytes;
	timeScale = calSize(newBytes);
	cout << "\n\t\tTimeScale: "; cout << timeScale;						boxSize -= newBytes;
	int seconds = calSize(newBytes);
	cout << "\n\t\tDuration: "; convertSecondsToHMS(seconds/timeScale);	boxSize -= newBytes;
	cout << "\n\t\tNext track ID : "; cout << calSize(nbytes);			boxSize -= nbytes;
	cout << "\n\t\tReserved : "; print(nbytes*6);						boxSize -= nbytes*6;
	skipData(boxSize);
}

void readTrak(){
	cout << "\n\tTrack box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\tName: ";	print(nbytes);								boxSize -= nbytes;

	readTkhd();
	readedts();
	readMdia();
}

void readTkhd(){
	bool version;
	cout << "\n\t\tTrack header box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\tName: ";	print(nbytes);								boxSize -= nbytes;
	cout << "\n\t\t\tVersion: "; version = printV(nbytes/4);				boxSize -= nbytes/4;
	cout << "\n\t\t\tFlags: "; print(3*nbytes/4);							boxSize -= 3*nbytes/4;

	size_t newBytes = nbytes;
	if(!version){ newBytes *= 2; }

	cout << "\n\t\t\tCreate_time: ";	calTime(newBytes);					boxSize -= newBytes;
	cout << "\n\t\t\tModification_time: "; calTime(newBytes);				boxSize -= newBytes;
	cout << "\n\t\t\tNext track ID : "; cout << calSize(nbytes);			boxSize -= nbytes;
	cout << "\n\t\t\tReserved : "; print(nbytes);								boxSize -= nbytes;
	int seconds = calSize(newBytes);
	cout << "\n\t\t\tDuration: "; convertSecondsToHMS(seconds/timeScale);	boxSize -= newBytes;
	skipData(boxSize);
}

void readedts(){
	cout << "\n\t\tEdit box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\tName: ";	print(nbytes);							boxSize -= nbytes;
	readelst();
}

void readelst(){
	cout << "\n\t\t\tEdit list box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\tName: ";	print(nbytes);							boxSize -= nbytes;
	skipData(boxSize);
}

void readMdia(){
	cout << "\n\t\tMedia box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\tName: ";	print(nbytes);							boxSize -= nbytes;
	readMdhd();
	readHdlr();
	readMinf();
}

void readMdhd(){
	bool version;
	cout << "\n\t\t\tMedia header box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\t\tName: ";	print(nbytes);								boxSize -= nbytes;
	cout << "\n\t\t\t\tVersion: "; version = printV(nbytes/4);				boxSize -= nbytes/4;
	cout << "\n\t\t\t\tFlags: "; print(3*nbytes/4);							boxSize -= 3*nbytes/4;

	size_t newBytes = nbytes;
	if(!version){ newBytes *= 2; }

	cout << "\n\t\t\t\tCreate_time: ";	calTime(newBytes);					boxSize -= newBytes;
	cout << "\n\t\t\t\tModification_time: "; calTime(newBytes);				boxSize -= newBytes;
	timeScale = calSize(newBytes);
	cout << "\n\t\t\t\tTimescale : "; cout << timeScale;					boxSize -= newBytes;
	int seconds = calSize(newBytes);
	cout << "\n\t\t\t\tDuration: "; convertSecondsToHMS(seconds/timeScale);	boxSize -= newBytes;
	cout << "\n\t\t\t\tLanguage: "; print(nbytes/2);						boxSize -= nbytes/2;
	cout << "\n\t\t\t\tQuality: "; print(nbytes/2);							boxSize -= nbytes/2;
	skipData(boxSize);
}

void readHdlr(){
	bool version;
	cout << "\n\t\t\tHander reference box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\t\tName: ";	print(nbytes);								boxSize -= nbytes;
	cout << "\n\t\t\t\tVersion: "; version = printV(nbytes/4);				boxSize -= nbytes/4;
	cout << "\n\t\t\t\tFlags: "; print(3*nbytes/4);							boxSize -= 3*nbytes/4;
	cout << "\n\t\t\t\tPre_defined: "; print(nbytes);						boxSize -= nbytes;
	cout << "\n\t\t\t\tHandler_type: "; print(nbytes);						boxSize -= nbytes;
	cout << "\n\t\t\t\tReserved: "; print(nbytes);							boxSize -= nbytes;
	cout << "\n\t\t\t\tName: "; print(boxSize);								boxSize -= nbytes;
}

void readMinf(){
	cout << "\n\t\t\tMedia information box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\t\tName: ";	print(nbytes);								boxSize -= nbytes;
	readDinf();
	readStbl();
}

void readStbl(){
	cout << "\n\t\t\t\tSample table box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\t\t\tName: ";	print(nbytes);							boxSize -= nbytes;
	readStdt();
	readStts();
}

void readDinf(){
	cout << "\n\t\t\t\tData information box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\t\t\tName: ";	print(nbytes);							boxSize -= nbytes;
	readDref();
}

void readDref(){
	cout << "\n\t\t\t\t\tData Reference Box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\t\t\t\tSize of box: " << boxSize << " bytes";			boxSize -= nbytes;
	cout << "\n\t\t\t\t\t\tName: ";	print(nbytes);							boxSize -= nbytes;
	skipData(boxSize);
}

void readStdt(){
	cout << "\n\t\t\t\tSample description box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\t\t\tName: ";	print(nbytes);							boxSize -= nbytes;
	cout << "\n\t\t\t\t\tSample Entry: ";	skipData(boxSize);				boxSize -= nbytes;
}

void readStts(){
	bool version;
	cout << "\n\t\t\t\tSample description box:";
	boxSize = calSize(nbytes);
	cout << "\n\t\t\t\t\tSize of box: " << boxSize << " bytes";				boxSize -= nbytes;
	cout << "\n\t\t\t\t\tName: ";	print(nbytes);							boxSize -= nbytes;
	cout << "\n\t\t\t\t\tVersion: "; version = printV(nbytes/4);			boxSize -= nbytes/4;
	cout << "\n\t\t\t\t\tFlags: "; print(3*nbytes/4);						boxSize -= 3*nbytes/4;
	int entry_count = calSize(nbytes);
	cout << "\n\t\t\t\t\tEntry_count: "; cout << entry_count;				boxSize -= nbytes;

	int i = 0; int Sample_count, Sample_duration;
	while (entry_count) {
		++i;
		cout << "\n\t\t\t\t\tSample_count ";		cout << i  <<  ": "; 
		Sample_count = calSize(nbytes); cout << Sample_count;				boxSize -= nbytes;
		cout << "\n\t\t\t\t\tSample_duration ";		cout << i  <<  ": ";
		Sample_duration = calSize(nbytes); cout << Sample_duration;			boxSize -= nbytes;
		entry_count--;
	}
}

int main()
{
    const char file_name[] = "aaa.mp4";
    try {
        file.open(file_name, ios::binary);

        readFtypBox();
        readMoovBox();
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}