#ifndef IVSENSORGPS_H
#define IVSENSORGPS_H

#include "Global.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "include/zmq.h"
#include "include/zmq_utils.h"

#include "include/json/json.h"

#include "VirtualSwitchZmq.h"

#include "include/Boost_Serial.h"

#include "Cycarr.h"
#include "boost/thread.hpp"

using namespace std;


//�������Ե�
#define MODE 4      //���Ե�---1ֱ��   2���   3ֹͣ

////////////////////////////////////////////////////////////////////////////


typedef struct PointF
{
	double x;
	double y;
}PointF;

typedef struct MatrixF
{
	int Num;
	PointF point[MAXSIZE];
}MatrixF;

//��Ҫ���������ݽṹ

typedef struct RoadPoint{
	double lon;      //����
	double lat;     //γ��
	unsigned char mode; //ģʽ��
	double azimuth;//�����
	double speed; //�ٶ�ֵ
	int rtkStatus;//rtk״̬
	int sateNum; //�ǵĸ���
}RoadPoint;


typedef struct Point2D
{
	double x;
	double y;
	double lat;
	double lon;
}Point2D;


typedef struct GpsPara
{
	std::string ZMQip;
	int ZMQport;
	int SerialPort;
	int SerialBaud;
	int RunMode;
	int IsSaveMapFile;
	int IsSaveMiniDisFile;
	double HeadOffset;
	double AxisOffset;
	std::string NaviProduct;
	std::string MapFileName;
	std::string LoadMapFileName;
	std::string OffLineMapFile;
	std::string MiniDisFileName;
}GpsPara;

class ivSensorGPS
{
public:

	int len;//����ʵ���յ�������
	unsigned char TempDataArray[350];//���ڻ�������С
	int ReceiverCurrentByteCount; //��ǰ�Ѿ����յ��ֽ���      
	RoadPoint rp;//��ǰ�����õ��ľ�γ������
	RoadPoint lastRp;//��һ�εľ�γ������
	unsigned char Rtk;//GPS��״̬
	unsigned short CurrentFrameID;//��ǰ֡ID
	int sendCount;//���ͱ�־λ
	FILE *fp;
	int ReceiverStatus;//����״̬��
	int CurrentFrameLength;//��ǰ֡����  

	int  mode; //��·����

	ifstream fpoint;
    
	CycArray<RoadPoint> szBuffer;

	//GPS���ݴ������
	RoadPoint nowPoint;
	Matrix matrixSend;
	MatrixF matrixSendF;

	int StartIndexRader;
	int LastIndex;
	vector<RoadPoint> roadPoints;   //��Ҫ�����ȡ
	unsigned int LastMapListIndex;//��һ��������ͼ����ʼ��
	FILE *fmini; //�洢�����

	GpsPara gpsPara;

private:
	//�������
	Boost_Serial* recvCom;

public:
	ivSensorGPS();
	~ivSensorGPS();

	//���������ļ����ƣ�Ĭ��Ϊconfig.json
	bool setParameter(std::string configFile = "config.json");

	//��ȡ�����ļ����ڵ����ƣ�GPS��������
	void readConfig(std::string configJson, std::string node, GpsPara &para); //�����Ҫѡ��

	//����GPS����
	void setGpsParam(GpsPara &data);

	void run();

	//���߳��Լ��ص�����
	
	int NaviDealKernel();
	int NaviColKernel();

	//���ݴ�����غ���

	//�����ͼ
	void loadMap();
	//������������
	void ZeroMatrix();
	//��������㲢դ��
	void rasterization();

	//����·��
	RoadPoint loadPoint();

private:

	//�����յ�������
	void ReceiverData(unsigned char* str, int len);

	//����BestPosa��������
	void ParseBestPosa();

	// ����Gptra��������
	void ParseGptra();

	//����Gpvtg��������
	void ParseGpvtg();
    
	//����Gpgga�������� 
	void ParseGpgga();

	//����Gprmc�������� 
	void ParseGprmc();

	// ����Gphpr�������� 
	void ParseGphpr();

	//���������תΪ��������
	Point2D BLH2XYZ(double B, double L, double H);

	//����������֮��ľ���
	double getDist2(Point2D p1, Point2D p2);

	//����ż����������߾���
	double GetL0InDegree(double dLIn);

	//����������γ��֮��ĽǶ�
	double getAngle(double lon1, double lat1, double lon2, double lat2);

	//����ת����
	double angToRad(double angle_d);  

	//����������γ��֮��ľ���
	double DisBetweenPoints(double latA, double lonA, double latB, double lonB);

	//ȥ����ͬ�ĵ�
	void DeleteSameDatas();

};

string int2String ( const int n );
//////////////////////////////////////////////////////////////////////////////////////////
//50ms���ڣ��б�Ҫ��Ūһ���߳��𣿸��˾����б�Ҫ�����ɼ�һ֡�������֪ͨ��һ���߳̿���������̴߳���˯��ģʽ��

#endif