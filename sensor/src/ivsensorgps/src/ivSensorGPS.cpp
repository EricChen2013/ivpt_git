

#include "ivSensorGPS.h"


static std::string ZMQip = "127.0.0.1";   //"ZMQ 端口IP"
static int ZMQport = 3000;                //"ZMQ 端口号",
static int SerialPort = 6;                //"GPS数据接收串口号"
static int SerialBaud = 115200;           //"GPS数据接收串口波特率"
static int RunMode = 1;                   //"选择导航程序运行模式(0:离线调试模式  1:运行模式)"
static int IsSaveMapFile = 1;             //"是否存储MapFileName(0:不存储 1：存储)"
static int IsSaveMiniDisFile = 1;         //"是否存储MiniDisFileName(0:不存储 1：存储)"
static double HeadOffset = 2.0;           //"接收机后天线距离车头的距离，单位为米" 
static double AxisOffset = 0.2;           //"接收机后天线偏离中轴线的距离，单位为米"
static std::string NaviProduct = "lianshi"; //"选择导航设备供应商  (0:司南导航设备  1:联适导航设备)"
static std::string MapFileName = "recodemaping.txt";//"存储地图文件名称,默认为recodemaping.txt"
static std::string LoadMapFileName = "mapint.txt";//"加载地图文件名称,默认为maping.txt"
static std::string OffLineMapFile = "loadpoint.txt"; //"离线调试地图文件名称,默认为loadpoint.txt"	
static std::string MiniDisFileName = "minidis.txt";   // "保存地图点与车最近的距离,默认为minidis.txt"	

void ivSensorGPS::setGpsParam(GpsPara &data)
{
	//用多个变量的目的是为了解决 GpsPara 变量的冲突问题
	if (data.MapFileName != "")
	{
		ZMQip = data.ZMQip;
		ZMQport = data.ZMQport;
		SerialPort = data.SerialPort;
		SerialBaud = data.SerialBaud;
		RunMode = data.RunMode;
		IsSaveMapFile = data.IsSaveMapFile;
		IsSaveMiniDisFile = data.IsSaveMiniDisFile;
		HeadOffset = data.HeadOffset;
		AxisOffset = data.AxisOffset;
		NaviProduct = data.NaviProduct;
		MapFileName = data.MapFileName;
		LoadMapFileName = data.LoadMapFileName;
		OffLineMapFile = data.OffLineMapFile;
		MiniDisFileName = data.MiniDisFileName;
	}
}

ivSensorGPS::ivSensorGPS()
{
	len = 0;
	ReceiverCurrentByteCount = 0;//当前帧已接收字节数
	ReceiverStatus = 0;//接收状态字
	CurrentFrameLength = 0;//当前帧长度            
	Rtk = 0x32;
	CurrentFrameID = 0;

	fp = fopen(MapFileName.c_str(),"a");
	mode = 1;
	sendCount = 0;
	fmini = fopen(MiniDisFileName.c_str(),"a"); 

	StartIndexRader = 0;
	LastIndex = 0;
	memset(&matrixSend,0,sizeof(Matrix));
	memset(&matrixSendF,0,sizeof(matrixSendF));
	memset(&rp,0,sizeof(rp));
	memset(&lastRp,0,sizeof(lastRp));

	if (RunMode == 0)
	{
		fpoint.open(OffLineMapFile.c_str());
	}

}

ivSensorGPS::~ivSensorGPS()
{
	fclose(fp);      //关闭文件
	fpoint.close(); //关闭文件
	recvCom->close(); //关闭串口
}

//zmq作为服务端，向外发布数据，所以需要bind端口，此端口为广播端口（可以认为是地址总线busNum）
//sleepTime我们可以作为定时器使用

bool ivSensorGPS::setParameter(std::string configFile)
{
	readConfig(configFile,"ivSensorGps",gpsPara);
	recvCom = new Boost_Serial(gpsPara.SerialBaud,gpsPara.SerialPort);
	setGpsParam(gpsPara);
	return true;
}



void ivSensorGPS::readConfig(std::string configJson, std::string node, GpsPara &para)
{
	Json::Reader reader;
	Json::Value root;

	//从文件中读取
	std::ifstream is;
	is.open(configJson.c_str(), ios::binary);
	if(reader.parse(is,root,FALSE))
	{
		//读取根节点信息
		para.ZMQip = root[node]["ZMQip"].asString();
		para.ZMQport = root[node]["ZMQport"].asInt();
		para.SerialPort = root[node]["SerialPort"].asInt();
		para.SerialBaud = root[node]["SerialBaud"].asInt();
		para.RunMode = root[node]["RunMode"].asInt();
		para.IsSaveMapFile = root[node]["IsSaveMapFile"].asInt();
		para.IsSaveMiniDisFile = root[node]["IsSaveMiniDisFile"].asInt();
		para.HeadOffset = root[node]["HeadOffset"].asDouble();
		para.AxisOffset = root[node]["AxisOffset"].asDouble();
		para.NaviProduct = root[node]["NaviProduct"].asString();
		para.MapFileName = root[node]["MapFileName"].asString();
		para.LoadMapFileName = root[node]["LoadMapFileName"].asString();
		para.OffLineMapFile = root[node]["OffLineMapFile"].asString();
		para.MiniDisFileName = root[node]["MiniDisFileName"].asString();
	}
	is.close();
}

void ivSensorGPS::run()
{
	boost::function<void()> f1 = boost::bind(&ivSensorGPS::NaviDealKernel,this);
	boost::thread m_DealDataThread(f1);

	Sleep(2000);
	if (RunMode == 1)
	{
		boost::function<void()>f2 = boost::bind(&ivSensorGPS::NaviColKernel,this);
		boost::thread m_ColDataThread(f2);
	}
}


int ivSensorGPS::NaviDealKernel()
{
	loadMap();
	cout << "GPS 初始化完成 ！ \n";

	VirtualSwitchZmqSend *vsSend = new VirtualSwitchZmqSend(ZMQport);

	GpsData gps_data;
	VirtualSwitchZmqSend *vsSend1 = new VirtualSwitchZmqSend(ZMQport+1);

	int sendLostCnt = 0;
	while(1)
	{
		int tick = GetTickCount();

		if (RunMode == 1)
		{
			if (szBuffer.isNew)
			{
				sendLostCnt = 0;
				nowPoint = szBuffer.Get();
			} 
			else
			{
				sendLostCnt++;
				if (sendLostCnt > 50) //30帧*100ms = 5s (数据没有)
				{
					//现在的做法是不向下发送数据
				}
				continue;  //我们需要添加没有数据的情况（可以发一个0，作为标志位）一个点
			}	     
			//当点的状态不好，就进行第二次的判断，此时不发数据给决策端

			if (!NaviProduct.compare("sinan"))
			{
				if (nowPoint.rtkStatus <= 3)
				{
					memset(&gps_data,0,sizeof(gps_data));
					gps_data.latitude = nowPoint.lat;
					gps_data.longitude = nowPoint.lon;
					gps_data.azimuth = nowPoint.azimuth;
					gps_data.speed = nowPoint.speed;
					gps_data.qf = nowPoint.rtkStatus;
					gps_data.satno = nowPoint.sateNum;
					gps_data.posstatus = 0;	   //未定位
					int sendNum1 = vsSend1->PubMsg("Gps_Data",gps_data);
					continue;
				}
			}
			else if (!NaviProduct.compare("lianshi"))
			{
				if (nowPoint.rtkStatus != 4)
				{
					memset(&gps_data,0,sizeof(gps_data));
					gps_data.latitude = nowPoint.lat;
					gps_data.longitude = nowPoint.lon;
					gps_data.azimuth = nowPoint.azimuth;
					gps_data.speed = nowPoint.speed;
					gps_data.qf = nowPoint.rtkStatus;
					gps_data.satno = nowPoint.sateNum;
					gps_data.posstatus = 0;	   //未定位
					int sendNum1 = vsSend1->PubMsg("Gps_Data",gps_data);
					continue;
				}
			}		
		} 
		else
		{
			//载入路点
			nowPoint = loadPoint();
			Sleep(50);
		}
			rasterization(); //找点并栅格化

			DeleteSameDatas();//去重复

			int sendNum = vsSend->PubMsg("Gps_Data",matrixSend);
			if (sendNum > 0)
			{
				cout << "publish GPS             "  << matrixSend.Num << "  data OK ! \n "; 
			} 
			else
			{
				cout << "publish GPS data Fail ! \n "; 
			}

			memset(&gps_data,0,sizeof(gps_data));
			gps_data.latitude = nowPoint.lat;
			gps_data.longitude = nowPoint.lon;
			gps_data.azimuth = nowPoint.azimuth;
			gps_data.speed = nowPoint.speed;
			gps_data.qf = nowPoint.rtkStatus;
			gps_data.satno = nowPoint.sateNum;
			gps_data.posstatus = 1;	  
			int sendNum1 = vsSend1->PubMsg("Gps_Data",gps_data);
			
			ZeroMatrix();
			Sleep(1);
			//cout << "SuseTime" << GetTickCount() - tick << endl;
	  }
	delete vsSend;
	delete vsSend1;
	return 0;
}

int ivSensorGPS::NaviColKernel()
{
	int i = 0;
	unsigned char str[1000]; 
	while(1)
	{
		memset(str,0,sizeof(str));
		int len = recvCom->read_from_serial((char*)&str,1000);
		if (len > 0 && len < 1000)
		{
			ReceiverData(str,len);//100ms内肯定已经全部都发送过来了，也就是说，我开辟一个大的buffer就OK了  包含 setEvent();
			//Sleep(10); //不能加在这,放在解析完的地方
		}

	}
	return 0;
}

/************************************************************************/
/* 目前的策略：直接对vector里面的数据进行处理，先找到一行数据，然后将vector里面的数据给删掉，记录个数*/
/* 通过个数将没用的从vector里面删掉。然后对这一行数据进行处理，根据报头进行处理，进入switch，然后根据逗号，*/
/* 一帧帧的截取我们所要的数据，数逗号的个数。每个解析我们可以写成一个函数的形式*/
/************************************************************************/
void ivSensorGPS::ReceiverData(unsigned char* str, int len)
{
	//采集并解析,放在两个线程里面，速度会快一点
	// int count = 0;
	// string content = "";
	for (int i = 0; i < len; ++i)
	{
		TempDataArray[ReceiverCurrentByteCount] = str[i];
		ReceiverCurrentByteCount++;
		if (ReceiverCurrentByteCount > 2)
		{
			if (TempDataArray[ReceiverCurrentByteCount - 2] == 0X0D && TempDataArray[ReceiverCurrentByteCount - 1] == 0X0A )  //每次都截取一段进行判断是否结束
			{
				//判断这一帧的前几个字母，进入到不同的处理程序
				if (ReceiverCurrentByteCount > 6)
				{
					if (!NaviProduct.compare("sinan"))
					{
						if (TempDataArray[0] == '#' && TempDataArray[1] == 'B' && TempDataArray[2] == 'E'&& TempDataArray[3] == 'S')
						{
							ParseBestPosa();
							sendCount = (sendCount + 1) % 3;
						}
						else if (TempDataArray[0] == '$' && TempDataArray[1] == 'G' && TempDataArray[2] == 'P'&& TempDataArray[3] == 'T')
						{
							ParseGptra();
							sendCount = (sendCount + 1) % 3;
						}
						else if (TempDataArray[0] == '$' && TempDataArray[1] == 'G' && TempDataArray[2] == 'P'&& TempDataArray[3] == 'V')
						{
							ParseGpvtg();
							sendCount = (sendCount + 1) % 3;
						}
					} 
					else if(!NaviProduct.compare("lianshi"))
					{
						if (TempDataArray[0] == '$' && TempDataArray[1] == 'G' && TempDataArray[2] == 'P'&& TempDataArray[3] == 'G')
						{
							ParseGpgga();
							sendCount = (sendCount + 1) % 3;
						}
						else if (TempDataArray[0] == '$' && TempDataArray[1] == 'G' && TempDataArray[2] == 'P'&& TempDataArray[3] == 'R')
						{
							ParseGprmc();
							sendCount = (sendCount + 1) % 3;
						}
						else if (TempDataArray[0] == '$' && TempDataArray[1] == 'P' && TempDataArray[2] == 'S'&& TempDataArray[3] == 'A')
						{
							ParseGphpr();
							sendCount = (sendCount + 1) % 3;
						}
					}						
					if(sendCount == 2)
					{
						rp.mode = MODE;//直道为1 ，弯道为2

						szBuffer.Set(rp);

						//cout << rp.rtkStatus<< endl;
						/************存储文件*************/
						//在存储之前，我们需要做一下筛选，0.2m一个点，间距不需要过密

						Point2D p1 = BLH2XYZ(rp.lat,rp.lon,0);
						Point2D p2 = BLH2XYZ(lastRp.lat,rp.lon,0);
						cout << "rtkstatus: " << rp.rtkStatus << " angle: " << rp.azimuth << " sateNum: "<< rp.sateNum << endl;

						bool isRecode = false;
						if (!NaviProduct.compare("sinan"))
						{
							if(getDist2(p1,p2) >= 0.01  && rp.rtkStatus >= 5)
							{
								isRecode = true;
							}
						} 
						else if(!NaviProduct.compare("lianshi"))
						{
							if (getDist2(p1,p2) >= 0.01 && rp.rtkStatus >= 2)  //2：伪距差分  4：固定解  5：浮动解
							{
								isRecode = true;
							}
						}

					if(isRecode) //只有rtk信号是正常的我们才记录数据 
					{
						//int start = clock();
						if (abs(rp.lon) > 200)
						{
							continue;
						}
						if (IsSaveMapFile == 1)
						{
							fp = fopen(MapFileName.c_str(),"a"); 
							SYSTEMTIME sys;
							GetLocalTime(&sys);
							char str[19];
							sprintf(str,"%02d/%02d %02d:%02d:%02d.%03d",sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
							fprintf(fp,"%.8lf,%.8lf,%d,%.8lf,%.8lf,%d,%d,%s\n",rp.lon,rp.lat,rp.mode,rp.azimuth,rp.speed,rp.rtkStatus,rp.sateNum,str);
							fclose(fp);
						}
						//cout << "time: " << clock() - start << endl;
						lastRp = rp;
					}
                       // Sleep(1);
				}
			  }
				ReceiverCurrentByteCount = 0;	//空行空格一定清零
			}
		}			
	}
}

void ivSensorGPS::ParseBestPosa()
{
	int strnum = 0;
	int DouNum = 0;
	string temp;
	for (int i = 0; i < ReceiverCurrentByteCount; ++i)
	{
		if (TempDataArray[i] == ',' )
		{
			strnum ++;
			stringstream stream;
			stream << temp;
			switch(strnum)
			{
			case 11:  
				{
					switch(temp[0])
					{
					case 'F': rp.rtkStatus = 2;          break;             //"FIXEDPOS"
					case 'S': 
						if (temp[1] == 'I')
						{
							rp.rtkStatus = 3; 
						} 
						else
						{
							rp.rtkStatus = 8; 
						}
						break;  //"SINGLE"  "SUPER WIDE-LANE"
					case 'P':rp.rtkStatus = 4;           break;  //"PSRDIFF"
					case 'N':
						if (temp[1] == 'O')
						{
							rp.rtkStatus = 1;
						}
						else if (temp[7] == 'F')
						{
							rp.rtkStatus = 5; 
						}else if (temp[7] == 'I')
						{
							rp.rtkStatus = 7; 
						}
						break;  //   "NONE"  "NARROW_FLOAT"  "NARRAW_INT"
					case 'W':rp.rtkStatus = 6;       break;  //"WIDE_INT"
					default: break;
					}
					stream.str(""); temp="";
				}
				break;
			case 12:  stream >> rp.lat;  stream.str(""); temp=""; break;
			case 13:  stream >> rp.lon;  stream.str(""); temp=""; break;
			default: stream.str(""); temp=""; break;
			}

		}
		else 
		{
			temp += TempDataArray[i];
		}	
	}
}

void ivSensorGPS::ParseGptra()
{
	int strnum = 0;
	int DouNum = 0;
	string temp;
	for (int i = 0; i < ReceiverCurrentByteCount; ++i)
	{
		if (TempDataArray[i] == ',' )
		{
			strnum ++;
			stringstream stream;
			stream << temp;
			switch(strnum)
			{
				case 3:  stream >> rp.azimuth;   stream.str(""); temp=""; break;
				case 6:  stream >> rp.rtkStatus; stream.str(""); temp=""; break;
				case 7:  stream >> rp.sateNum;   stream.str(""); temp=""; break;
				default: stream.str(""); temp=""; break;
			}
		}
		else 
		{
			temp += TempDataArray[i];
		}	
	}
}

void ivSensorGPS::ParseGpvtg()
{
	int strnum = 0;
	int DouNum = 0;
	string temp;
	for (int i = 0; i < ReceiverCurrentByteCount; ++i)
	{
		if (TempDataArray[i] == ',' )
		{
			strnum ++;
			stringstream stream;
			stream << temp;
			switch(strnum)
			{
			case 8:  stream >> rp.speed;  stream.str(""); temp=""; break;
			default: stream.str(""); temp=""; break;
			}
		}
		else 
		{
			temp += TempDataArray[i];
		}	
	}
}

void ivSensorGPS::ParseGpgga()
{
	int strnum = 0;
	int DouNum = 0;
	string temp;
	double lon = 0;
	double lat = 0;
	int inter = 0;
	for (int i = 0; i < ReceiverCurrentByteCount; ++i)
	{
		if (TempDataArray[i] == ',' )
		{
			strnum ++;
			stringstream stream;
			stream << temp;
			switch(strnum)
			{
			case 3:  
				stream >> lat;  
				inter = (int)(lat) / 100;
				lat = lat - inter*100;
				lat = lat/60.0 + inter;
				rp.lat = lat;
				stream.str(""); temp=""; break;
			case 5:
				stream >> lon;
				inter = (int)(lon) / 100;
				lon = lon - inter*100;
				lon = lon/60.0 + inter;
				rp.lon = lon;
				stream.str(""); temp=""; break;
			case 7:
				stream >> rp.rtkStatus;
				stream.str(""); temp=""; break;
			case 8:
				stream >> rp.sateNum;
				stream.str(""); temp=""; break;
			default: stream.str(""); temp=""; break;
			}
		}
		else 
		{
			temp += TempDataArray[i];
		}	
	}
}

void ivSensorGPS::ParseGprmc()
{
	int strnum = 0;
	int DouNum = 0;
	string temp;

	for (int i = 0; i < ReceiverCurrentByteCount; ++i)
	{
		if (TempDataArray[i] == ',' )
		{
			strnum ++;
			stringstream stream;
			stream << temp;
			switch(strnum)
			{
			case 8:  
				stream >> rp.speed;  
				rp.speed *= 1.852; //一节 = 一海里 = 1.852km/h
				stream.str(""); temp=""; break;
			default: stream.str(""); temp=""; break;
			}
		}
		else 
		{
			temp += TempDataArray[i];
		}	
	}k
}

void ivSensorGPS::ParseGphpr()
{
	int strnum = 0;
	int DouNum = 0;
	string temp;

	for (int i = 0; i < ReceiverCurrentByteCount; ++i)
	{
		if (TempDataArray[i] == ',' )
		{
			strnum ++;
			stringstream stream;
			stream << temp;
			switch(strnum)
			{
			case 4:  
				stream >> rp.azimuth;  
				stream.str(""); temp=""; break;
			default: stream.str(""); temp=""; break;
			}
		}
		else 
		{
			temp += TempDataArray[i];
		}	
	}
}

double ivSensorGPS::angToRad(double angle_d)  ///度数转弧度
{
    double Pi = 3.1415926535898;
    double rad;
    rad = angle_d * Pi / 180;
    return rad;
}

double ivSensorGPS::DisBetweenPoints(double latA, double lonA, double latB, double lonB)
{
    //定义pi
    const double PI = 3.1415926535897932384626433832795;
    //定义地球半径，单位为米
    const double R = 6371004;
    //公式
    double C = sin(latA) * sin(latB) * cos(lonA - lonB) + cos(latA) * cos(latB);

    double distance = R * acos(C) * PI / 180;

    return distance;
}
 //根据平面坐标计算距离
  double ivSensorGPS::getDist2(Point2D p1, Point2D p2)
    {
        return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    }

   double ivSensorGPS::GetL0InDegree(double dLIn)
    {
        //3°带求带号及中央子午线经度(d的形式)
        //具体公式请参阅《常用大地坐标系及其变换》朱华统，解放军出版社138页
        double L = dLIn;//d.d
        double L_ddd_Style = L;
        double ZoneNumber = (int)((L_ddd_Style - 1.5) / 3.0) + 1;
        double L0 = ZoneNumber * 3.0;//degree
        return L0;
    }
 /************************************************************************/
   /* B: lat纬度
    * L: lon经度
    * H: height
/************************************************************************/
 Point2D ivSensorGPS::BLH2XYZ(double B, double L, double H)
    {
        double N, E, h;
        double L0 = GetL0InDegree(L);//根据经度求中央子午线经度
        Point2D pt2d;
        double a = 6378245.0;            //地球半径  北京6378245
        double F = 298.257223563;        //地球扁率
        double iPI = 0.0174532925199433; //2pi除以360，用于角度转换

        double f = 1 / F;
        double b = a * (1 - f);
        double ee = (a * a - b * b) / (a * a);
        double e2 = (a * a - b * b) / (b * b);
        double n = (a - b) / (a + b), n2 = (n * n), n3 = (n2 * n), n4 = (n2 * n2), n5 = (n4 * n);
        double al = (a + b) * (1 + n2 / 4 + n4 / 64) / 2;
        double bt = -3 * n / 2 + 9 * n3 / 16 - 3 * n5 / 32;
        double gm = 15 * n2 / 16 - 15 * n4 / 32;
        double dt = -35 * n3 / 48 + 105 * n5 / 256;
        double ep = 315 * n4 / 512;

        B = B * iPI;
        L = L * iPI;
        L0 = L0 * iPI;

        double l = L - L0, cl = (cos(B) * l), cl2 = (cl * cl), cl3 = (cl2 * cl), cl4 = (cl2 * cl2), cl5 = (cl4 * cl), cl6 = (cl5 * cl), cl7 = (cl6 * cl), cl8 = (cl4 * cl4);
        double lB = al * (B + bt * sin(2 * B) + gm * sin(4 * B) + dt * sin(6 * B) + ep * sin(8 * B));
        double t = tan(B), t2 = (t * t), t4 = (t2 * t2), t6 = (t4 * t2);
        double Nn = a / sqrt(1 - ee * sin(B) * sin(B));
        double yt = e2 * cos(B) * cos(B);
        N = lB;
        N += t * Nn * cl2 / 2;
        N += t * Nn * cl4 * (5 - t2 + 9 * yt + 4 * yt * yt) / 24;
        N += t * Nn * cl6 * (61 - 58 * t2 + t4 + 270 * yt - 330 * t2 * yt) / 720;
        N += t * Nn * cl8 * (1385 - 3111 * t2 + 543 * t4 - t6) / 40320;

        E = Nn * cl;
        E += Nn * cl3 * (1 - t2 + yt) / 6;
        E += Nn * cl5 * (5 - 18 * t2 + t4 + 14 * yt - 58 * t2 * yt) / 120;
        E += Nn * cl7 * (61 - 479 * t2 + 179 * t4 - t6) / 5040;

        E += 500000;
        //if (nFlag == 1)
        //{
        //    //UTM投影
        //    N = 0.9996 * N;
        //    E = 0.9996 * (E - 500000.0) + 500000.0;//Get y
        //}
       /* if (nFlag == 2)
        {*/
            //UTM投影
            N = 0.9999 * N;
            E = 0.9999 * (E - 500000.0) + 250000.0;//Get y
        //}

        //原
        //pt2d.x = N;
        //pt2d.y = E;
        //
        pt2d.x = E;
        pt2d.y = N;
        h = H;

        return pt2d;
    }

 //返回向量12和真北的夹角
 double ivSensorGPS::getAngle(double lon1, double lat1, double lon2, double lat2)
    {
        double angle;
        double averageLat = (lat1 + lat2) / 2;
		double PI = 3.141592654;
        if (abs(lat1 - lat2) <= 1e-6)
        {
            angle = 90;
            if (lon1 > lon2)
            {

                angle = angle + 180;
            }
        }
        else
        {
            angle = atan((lon1 - lon2) * cos(angToRad(averageLat)) / (lat1 - lat2)) * 180 / PI;
            if (lat1 > lat2)//?
            //if (lon1 > lon2)
            {

                angle = angle + 180;
            }
            if (angle < 0)
            {
                angle = 360 + angle;
            }
        }
        //if (lat1 > lat2)//?
        ////if (lon1 > lon2)
        //{
                
        //    angle = angle + 180;
        //}
        //if (angle < 0)
        //{
        //    angle = 360 + angle;
        //}

        return angle;
    }

 RoadPoint ivSensorGPS::loadPoint()
 {
	 string line,temp;
	 RoadPoint rp;
	 if(!fpoint.eof())
	 {
		 getline(fpoint,line);
		 temp="";
		 memset(&rp,0,sizeof(RoadPoint));
		 int strnum = 0;
		 for (unsigned int i = 0; i < line.size(); ++i)
		 {
			 if (line[i] == ',' )
			 {
				 stringstream stream;
				 stream << temp;
				 switch(strnum)
				 {
				 case 0:  stream >> rp.lon;  stream.str(""); temp=""; break;
				 case 1:  stream >> rp.lat;  stream.str(""); temp=""; break;
				 case 2:  stream >> rp.mode; stream.str(""); temp=""; break;
			     case 3:  stream >> rp.azimuth; stream.str(""); temp=""; break;
				 case 4:  stream >> rp.speed; stream.str(""); temp=""; break;
				 case 5:  stream >> rp.rtkStatus; stream.str(""); temp=""; break;
				 case 6:  stream >> rp.sateNum; stream.str(""); temp=""; break;
				 default: stream.str(""); temp="";break;
				 }
				 strnum ++;
			 }
			 else 
			 {
				 temp += line[i];
			 }
		 }
        return rp;
	 }
	 else
	 {
		 fpoint.close();
		 fpoint.open(OffLineMapFile.c_str());
		 rp.lat = 0;
		 rp.lon = 0;
		 return rp;
	 }
 }

 string int2String ( const int n )
 {
	 std::stringstream newstr;
	 newstr<<n;
	 return newstr.str();
 }

 //主要就是解析文件，然后将数据加载到list当中
 void ivSensorGPS::loadMap()
 {
	 ifstream fin(LoadMapFileName.c_str());
	 string line,temp;
	 while(getline(fin,line))
	 {
		 temp="";
		 RoadPoint rp;
		 memset(&rp,0,sizeof(RoadPoint));
		 int strnum = 0;
		 for (unsigned int i = 0; i < line.size(); ++i)
		 {
			 if (line[i] == ',' )
			 {
				 stringstream stream;
				 stream << temp;
				 switch(strnum)
				 {
					 case 0:  stream >> rp.lon;  stream.str(""); temp=""; break;
					 case 1:  stream >> rp.lat;  stream.str(""); temp=""; break;
					 case 2:  stream >> rp.mode; stream.str(""); temp=""; break;
					 case 3:  stream >> rp.azimuth; stream.str(""); temp=""; break;
					 default: stream.str(""); temp="";break;
				 }
				 strnum ++;
			 }
			 else 
			 {
				 temp += line[i];
			 }
		 }
		// rp.mode = atoi(temp.c_str());
		 roadPoints.push_back(rp);
	 }
	 fin.close();
 }

 void ivSensorGPS::rasterization()
 {
	 Point2D nowPosition = BLH2XYZ(nowPoint.lat, nowPoint.lon, 0);//将车当前位置的经纬度坐标转换为xy坐标
	 Point2D tempPosition;

	 double distance;
	 double angle;
	 double x;
	 double y;
	 double lasty = -11;

	 int numY=0; //当前点纵坐标大于上一点纵坐标数统计
	 bool flag = true;

	 //寻找最近的点并保存
	 double miniDis = 10000;
	 double Minidis = 0;
	 double PI = 3.141592654;

	 for(unsigned int i = LastIndex; i < roadPoints.size(); ++i) //如果需要重新搜索，只需要置LastIndex为0（目前只考虑正向搜索，不考虑反向搜索）
	 {
		 tempPosition = BLH2XYZ(roadPoints[i].lat, roadPoints[i].lon, 0);
		 distance = getDist2(nowPosition,tempPosition);
		 angle = getAngle(nowPoint.lon, nowPoint.lat,roadPoints[i].lon, roadPoints[i].lat); //根据经纬度求两点组成的向量与真北的夹角，0-360度
		 angle = angle - nowPoint.azimuth;

		 /************************************************************************/
		 /* 这里的正负号可能和导航放的位置有关系                                 */
		 /************************************************************************/

		 x = distance * sin(angle * PI / 180.0) - AxisOffset;
		 y = distance * cos(angle * PI / 180.0) - HeadOffset;

		 //用y值做判断也可以,当前点的纵坐标要大于上一次的 往下寻找，不往后寻找
		 if (x >= -WIDTH_REAL && x <= WIDTH_REAL && y >= 0 && y <= HEIGHT_REAL) //&& y > lasty（加入它就没有回头路了）  （-10m到10m) (0 到 60)
		 {

			 lasty = y; //将当前点的y值赋值给Lasty

			 if(flag) //第一次找到，记下满足条件点的y坐标
			 {
				 lasty = y; //将当前点的y值赋值给Lasty
			 }
			 numY++;
			 LastIndex = i;

			 //找到的第二个满足条件的点，作为下一次的起点用
			 if(flag && numY > 1)
			 {
				 StartIndexRader = LastIndex - 1;//记录第一次的起始位置
				 lasty = -11;
				 flag = false;
			 }
			 //保证 matrixSend里面发送的点是准确的。（说明第一次找到一个满足条件的点，看看下一次是不是还是满足条件。要是满足的话，执行到下面的发送部分。
			 if(flag)
			 {
				 continue;
			 }
			 unsigned int n = CAR_POSITION_H - (int)(y * RESOLUTION_REAL);  //100/20   100/10
			 unsigned int m = CAR_POSITION_W + (int)(x * RESOLUTION_REAL);

			 if ((n > 0 && n <= CAR_POSITION_H) && (m > 0 && m <= WIDTH)) //后面数据直接省略
			 {
				 matrixSendF.point[matrixSendF.Num].x = y;
				 matrixSendF.point[matrixSendF.Num].y = x;
				 matrixSendF.Num++;

				 matrixSend.point[matrixSend.Num].x = n;                     //x 是纵坐标，y是横坐标
				 matrixSend.point[matrixSend.Num].y = m;
				 matrixSend.point[matrixSend.Num].value = roadPoints[i].mode; //属性点的使用
				 matrixSend.point[matrixSend.Num].U = 0;
				 matrixSend.Num++;

				 if (IsSaveMiniDisFile)
				 {
					 if(distance < miniDis) //寻找车距离轨迹最近的距离点
					 {
						 miniDis = distance;
						 if(nowPosition.x < tempPosition.x)
						 {
							 Minidis = distance;
						 }
						 else
						 {
							 Minidis = -distance;
						 }
					 }
				 }
			 }
		 }
		 else if (numY == 1) //如果第一次进上面的if，第二次没有进上面的if，那么将参数恢复到默认值。
		 {
			 lasty = -11;
			 numY = 0;
		 }
		 else if(!flag && numY > 1) // 当找到咱们要求的范围内的点后，突然有一个点不满足时，我们就认为不需要再找了
		 {
			 break;
		 }
	 }
	 if(StartIndexRader > 5)
	 {
		 LastIndex = StartIndexRader - 5;//下一次查找的起始点
	 }
	 else
	 {
		 LastIndex = 0;
	 }
	 StartIndexRader = 0;

	if (IsSaveMiniDisFile)
	{
		if(miniDis < 1000)
		{
			fprintf(fmini,"%.8lf\n",Minidis);
			fclose(fmini);
			fmini = fopen(MiniDisFileName.c_str(),"a"); 
			//cout << "dismini     " << Minidis << endl;
		}
	}
	
 }

 void ivSensorGPS::ZeroMatrix()
 {
	 //memset(&matrixSend,0,sizeof(Matrix));
	 matrixSend.Num = 0;
	 matrixSendF.Num = 0;
 }

 void ivSensorGPS::DeleteSameDatas()
 {
#pragma  region  去重复 			
	 /************************************************************************/
	 /*                                  去重复                              */
	 /************************************************************************/
	 Matrix matrix;
	 memset(&matrix,0,sizeof(Matrix));
	 matrix.Num = 0;
	 int cur = 0;
	 int flag = 0;
	 for (int i = 0; i < matrixSend.Num; ++i)
	 {
		 for (int k = 0; k <= cur; ++k)
		 {
			 if ((matrixSend.point[i].x == matrix.point[k].x) && (matrixSend.point[i].y == matrix.point[k].y))
			 {
				 //if (mapProcessor->matrixSend.point[i].value != 128)//若相等，不用做处理,原来的是多少就为多少。若不等，以130为主
				 {
					 matrix.point[k].value = matrixSend.point[i].value;
				 }

				 flag = 1;  //从新开辟的数组内，找到一个与你匹配的数组里面的数据有重复，则赋值，并退出本次循环
				 break;
			 }
		 }
		 if(flag == 0) //如果没有重复
		 {
			 matrix.point[cur] = matrixSend.point[i];
			 cur++;
		 }
		 else
		 {
			 flag = 0;
		 }

	 }
	 matrix.Num = cur;
	 matrixSend = matrix;
	 //////////////////////////////////////////////////////////////////////////
#pragma  endregion
 }
