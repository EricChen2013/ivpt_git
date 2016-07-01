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


//导航属性点
#define MODE 4      //属性点---1直道   2弯道   3停止

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

//需要解析的数据结构

typedef struct RoadPoint{
	double lon;      //经度
	double lat;     //纬度
	unsigned char mode; //模式点
	double azimuth;//航向角
	double speed; //速度值
	int rtkStatus;//rtk状态
	int sateNum; //星的个数
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

	int len;//串口实际收到的数据
	unsigned char TempDataArray[350];//串口缓冲区大小
	int ReceiverCurrentByteCount; //当前已经接收的字节数      
	RoadPoint rp;//当前解析得到的经纬度坐标
	RoadPoint lastRp;//上一次的经纬度坐标
	unsigned char Rtk;//GPS的状态
	unsigned short CurrentFrameID;//当前帧ID
	int sendCount;//发送标志位
	FILE *fp;
	int ReceiverStatus;//接收状态字
	int CurrentFrameLength;//当前帧长度  

	int  mode; //道路类型

	ifstream fpoint;
    
	CycArray<RoadPoint> szBuffer;

	//GPS数据处理相关
	RoadPoint nowPoint;
	Matrix matrixSend;
	MatrixF matrixSendF;

	int StartIndexRader;
	int LastIndex;
	vector<RoadPoint> roadPoints;   //需要随机存取
	unsigned int LastMapListIndex;//上一次搜索地图的起始点
	FILE *fmini; //存储最近点

	GpsPara gpsPara;

private:
	//串口相关
	Boost_Serial* recvCom;

public:
	ivSensorGPS();
	~ivSensorGPS();

	//设置配置文件名称，默认为config.json
	bool setParameter(std::string configFile = "config.json");

	//读取配置文件，节点名称，GPS配置数据
	void readConfig(std::string configJson, std::string node, GpsPara &para); //这个需要选择

	//设置GPS数据
	void setGpsParam(GpsPara &data);

	void run();

	//子线程以及回调函数
	
	int NaviDealKernel();
	int NaviColKernel();

	//数据处理相关函数

	//载入地图
	void loadMap();
	//发送数据清零
	void ZeroMatrix();
	//加载最近点并栅格化
	void rasterization();

	//载入路点
	RoadPoint loadPoint();

private:

	//解析收到的数据
	void ReceiverData(unsigned char* str, int len);

	//解析BestPosa类型数据
	void ParseBestPosa();

	// 解析Gptra类型数据
	void ParseGptra();

	//解析Gpvtg类型数据
	void ParseGpvtg();
    
	//解析Gpgga类型数据 
	void ParseGpgga();

	//解析Gprmc类型数据 
	void ParseGprmc();

	// 解析Gphpr类型数据 
	void ParseGphpr();

	//将大地坐标转为车体坐标
	Point2D BLH2XYZ(double B, double L, double H);

	//计算两个点之间的距离
	double getDist2(Point2D p1, Point2D p2);

	//求带号及中央子午线经度
	double GetL0InDegree(double dLIn);

	//计算两个经纬度之间的角度
	double getAngle(double lon1, double lat1, double lon2, double lat2);

	//度数转弧度
	double angToRad(double angle_d);  

	//计算两个经纬度之间的距离
	double DisBetweenPoints(double latA, double lonA, double latB, double lonB);

	//去除相同的点
	void DeleteSameDatas();

};

string int2String ( const int n );
//////////////////////////////////////////////////////////////////////////////////////////
//50ms以内，有必要在弄一个线程吗？个人觉得有必要，当采集一帧数据完毕通知另一个线程开启，这边线程处于睡觉模式。

#endif