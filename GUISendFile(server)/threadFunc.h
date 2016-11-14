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
	//����  ������ ���� ����.
	char buff[BUFFSIZE];
	ifstream ifs;

	//������ �̸��� ����ũ�������� ����ִ� ����ü
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
	//��Ÿ������ ����.
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
		//SendMessage(h_ProgressbarCtrl, PBM_SETPOS, (WPARAM)40,0); --- �ۼ������� ä���.;
		/*cout << sendDataLen << "����Ʈ ����" << endl;
		cout << ((float)sendDataLen / (float)metaData.len) * 100 << "�ۼ�Ʈ ����" << endl;*/
		//ShowTransferInfo(metaData, sendDataLen);
	}
	chrono::system_clock::time_point endTime = chrono::system_clock::now();
	chrono::seconds second = chrono::duration_cast<chrono::seconds>(endTime - startTime);

	/*cout << "�Ѱɸ� �ð�(��)" << second.count() << endl;
	cout << "��" << sendNum << "���� send()�Լ� ȣ��" << endl;;
	cout << "��" << sendDataLen << "����Ʈ�� ����" << endl;;
	cout << "�����մϴ�" << endl;
	cout << "���ε�ӵ� ���(��):" << (float)metaData.len / (float)second.count() << "����Ʈ" << endl;
	cout << "���ۻ�����:" << BUFFSIZE << endl;*/
	char temp[1000];

	wsprintf(temp, "�Ѱɸ� �ð�(��) : %d %d",second.count());
	MessageBox(0, temp, "�Ϸ�", 0);
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
//	cout << "�����̸�:" << mydata.name << endl;
//	cout << "�� ����ũ��" << mydata.len << "����Ʈ" << endl;
//	cout << recvedLen << "����Ʈ ����or����" << endl;
//	cout << (((float)recvedLen / (float)mydata.len)) * 100 << "%" << endl;
//}

int GetFilesize(const char* path)
{
	std::ifstream ifs(path, std::ios::in | std::ios::binary);
	int size = 0;

	if (ifs.is_open() == 0)
		MessageBox(0, "������ �������� ����", 0, 0);

	ifs.seekg(0, std::ios::end);
	size = ifs.tellg();
	//std::istream::pos_type pt = ifs.tellg();
	ifs.close();

	return size;
}
