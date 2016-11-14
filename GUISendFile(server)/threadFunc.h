#pragma once
#include "stdafx.h"
#include<chrono>
#include<fstream>
#include"CMyFunc.h"
#include<CommCtrl.h>
using namespace std;

void ThreadFunc(const HWND hPrgrsb, const SOCKADDR_IN addr, char* pFilename);
//void ShowTransferInfo(const MYDATA& mydata, const int& recvedLen);
SOCKADDR_IN InitSockAddrIPv4(const char* ipAddr, const int& port);
int GetFilesize(const char* path);


typedef struct my_data
{
	char name[100];
	int len;
}MYDATA;
#pragma pack
void ThreadFunc(const HWND hPrgrsb, const SOCKADDR_IN addr, char* pFilename)
{
	const int BUFFSIZE = 10000;
	const int PORT = 9000;
	int retval;
	int sendDataLen = 0;
	//보낼  파일을 담을 버퍼.
	char buff[BUFFSIZE];
	ifstream ifs;

	//파일의 이름과 파일크기정보가 들어있는 구조체
	MYDATA	metaData;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		CMyFunc::err_quit("wsaStartUp()");
	}
	SOCKET clientsocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientsocket == INVALID_SOCKET)
	{
		CMyFunc::err_display("socket");
	}
	metaData.len = GetFilesize(pFilename);
	ifs.open(pFilename, ios::binary | ios::in);
	//메타데이터 설정.
	strcpy(metaData.name, pFilename);

	int sendNum = 0;
	SOCKADDR_IN serverAddr = InitSockAddrIPv4(inet_ntoa(addr.sin_addr), PORT);

	retval = connect(clientsocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		CMyFunc::err_quit("connect()");
	}
	int sendBuffsize = 0;
	chrono::system_clock::time_point startTime = chrono::system_clock::now();
	retval = send(clientsocket, (char*)&metaData, sizeof(metaData), 0);
	while (1)
	{
		sendBuffsize = ifs.read(buff, BUFFSIZE).gcount();

		//fileState = getCharToBuff(ifs, buff, BUFFSIZE);
		retval = send(clientsocket, (char*)&sendBuffsize, sizeof(sendBuffsize), 0);
		if (retval == SOCKET_ERROR)
		{
			CMyFunc::err_display("send");
			break;
		}
		else if (retval == 0)
		{
			break;
		}

		SendMessage(hPrgrsb, PBM_SETPOS, (WPARAM)(int)(((float)sendDataLen / (float)metaData.len) * 100), 0);

		sendNum++;
		sendDataLen += retval;

		retval = send(clientsocket, buff, sendBuffsize, 0);
		if (retval == SOCKET_ERROR)
		{
			CMyFunc::err_display("send");
			break;
		}
		else if (retval == 0)
		{
			break;
		}
		sendNum++;
		sendDataLen += retval;
		//SendMessage(h_ProgressbarCtrl, PBM_SETPOS, (WPARAM)40,0); --- 퍼센테이지 채우기.;
		/*cout << sendDataLen << "바이트 보냄" << endl;
		cout << ((float)sendDataLen / (float)metaData.len) * 100 << "퍼센트 보냄" << endl;*/
		//ShowTransferInfo(metaData, sendDataLen);
	}
	chrono::system_clock::time_point endTime = chrono::system_clock::now();
	chrono::seconds second = chrono::duration_cast<chrono::seconds>(endTime - startTime);

	/*cout << "총걸린 시간(초)" << second.count() << endl;
	cout << "총" << sendNum << "번의 send()함수 호출" << endl;;
	cout << "총" << sendDataLen << "바이트를 보냄" << endl;;
	cout << "종료합니다" << endl;
	cout << "업로드속도 평균(초):" << (float)metaData.len / (float)second.count() << "바이트" << endl;
	cout << "버퍼사이즈:" << BUFFSIZE << endl;*/
	char temp[1000];

	wsprintf(temp, "총걸린 시간(초) : %d %d",second.count());
	MessageBox(0, temp, "완료", 0);
	ifs.close();

	closesocket(clientsocket);
	WSACleanup();

}



SOCKADDR_IN InitSockAddrIPv4(const char* ipAddr, const int& port)
{
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_addr.s_addr = inet_addr(ipAddr);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);

	return serverAddr;
}
//
//void ShowTransferInfo(const MYDATA& mydata, const int& recvedLen)
//{
//	cout << "파일이름:" << mydata.name << endl;
//	cout << "총 파일크기" << mydata.len << "바이트" << endl;
//	cout << recvedLen << "바이트 전송or수신" << endl;
//	cout << (((float)recvedLen / (float)mydata.len)) * 100 << "%" << endl;
//}

int GetFilesize(const char* path)
{
	std::ifstream ifs(path, std::ios::in | std::ios::binary);
	int size = 0;

	if (ifs.is_open() == 0)
		MessageBox(0, "파일이 존재하지 않음", 0, 0);

	ifs.seekg(0, std::ios::end);
	size = ifs.tellg();
	//std::istream::pos_type pt = ifs.tellg();
	ifs.close();

	return size;
}
