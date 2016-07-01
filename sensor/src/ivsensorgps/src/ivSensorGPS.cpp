

#include "ivSensorGPS.h"


static std::string ZMQip = "127.0.0.1";   //"ZMQ �˿�IP"
static int ZMQport = 3000;                //"ZMQ �˿ں�",
static int SerialPort = 6;                //"GPS���ݽ��մ��ں�"
static int SerialBaud = 115200;           //"GPS���ݽ��մ��ڲ�����"
static int RunMode = 1;                   //"ѡ�񵼺���������ģʽ(0:���ߵ���ģʽ  1:����ģʽ)"
static int IsSaveMapFile = 1;             //"�Ƿ�洢MapFileName(0:���洢 1���洢)"
static int IsSaveMiniDisFile = 1;         //"�Ƿ�洢MiniDisFileName(0:���洢 1���洢)"
static double HeadOffset = 2.0;           //"���ջ������߾��복ͷ�ľ��룬��λΪ��" 
static double AxisOffset = 0.2;           //"���ջ�������ƫ�������ߵľ��룬��λΪ��"
static std::string NaviProduct = "lianshi"; //"ѡ�񵼺��豸��Ӧ��  (0:˾�ϵ����豸  1:���ʵ����豸)"
static std::string MapFileName = "recodemaping.txt";//"�洢��ͼ�ļ�����,Ĭ��Ϊrecodemaping.txt"
static std::string LoadMapFileName = "mapint.txt";//"���ص�ͼ�ļ�����,Ĭ��Ϊmaping.txt"
static std::string OffLineMapFile = "loadpoint.txt"; //"���ߵ��Ե�ͼ�ļ�����,Ĭ��Ϊloadpoint.txt"	
static std::string MiniDisFileName = "minidis.txt";   // "�����ͼ���복����ľ���,Ĭ��Ϊminidis.txt"	

void ivSensorGPS::setGpsParam(GpsPara &data)
{
	//�ö��������Ŀ����Ϊ�˽�� GpsPara �����ĳ�ͻ����
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
	ReceiverCurrentByteCount = 0;//��ǰ֡�ѽ����ֽ���
	ReceiverStatus = 0;//����״̬��
	CurrentFrameLength = 0;//��ǰ֡����            
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
	fclose(fp);      //�ر��ļ�
	fpoint.close(); //�ر��ļ�
	recvCom->close(); //�رմ���
}

//zmq��Ϊ����ˣ����ⷢ�����ݣ�������Ҫbind�˿ڣ��˶˿�Ϊ�㲥�˿ڣ�������Ϊ�ǵ�ַ����busNum��
//sleepTime���ǿ�����Ϊ��ʱ��ʹ��

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

	//���ļ��ж�ȡ
	std::ifstream is;
	is.open(configJson.c_str(), ios::binary);
	if(reader.parse(is,root,FALSE))
	{
		//��ȡ���ڵ���Ϣ
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
	cout << "GPS ��ʼ����� �� \n";

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
				if (sendLostCnt > 50) //30֡*100ms = 5s (����û��)
				{
					//���ڵ������ǲ����·�������
				}
				continue;  //������Ҫ���û�����ݵ���������Է�һ��0����Ϊ��־λ��һ����
			}	     
			//�����״̬���ã��ͽ��еڶ��ε��жϣ���ʱ�������ݸ����߶�

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
					gps_data.posstatus = 0;	   //δ��λ
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
					gps_data.posstatus = 0;	   //δ��λ
					int sendNum1 = vsSend1->PubMsg("Gps_Data",gps_data);
					continue;
				}
			}		
		} 
		else
		{
			//����·��
			nowPoint = loadPoint();
			Sleep(50);
		}
			rasterization(); //�ҵ㲢դ��

			DeleteSameDatas();//ȥ�ظ�

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
			ReceiverData(str,len);//100ms�ڿ϶��Ѿ�ȫ�������͹����ˣ�Ҳ����˵���ҿ���һ�����buffer��OK��  ���� setEvent();
			//Sleep(10); //���ܼ�����,���ڽ�����ĵط�
		}

	}
	return 0;
}

/************************************************************************/
/* Ŀǰ�Ĳ��ԣ�ֱ�Ӷ�vector��������ݽ��д������ҵ�һ�����ݣ�Ȼ��vector��������ݸ�ɾ������¼����*/
/* ͨ��������û�õĴ�vector����ɾ����Ȼ�����һ�����ݽ��д������ݱ�ͷ���д�������switch��Ȼ����ݶ��ţ�*/
/* һ֡֡�Ľ�ȡ������Ҫ�����ݣ������ŵĸ�����ÿ���������ǿ���д��һ����������ʽ*/
/************************************************************************/
void ivSensorGPS::ReceiverData(unsigned char* str, int len)
{
	//�ɼ�������,���������߳����棬�ٶȻ��һ��
	// int count = 0;
	// string content = "";
	for (int i = 0; i < len; ++i)
	{
		TempDataArray[ReceiverCurrentByteCount] = str[i];
		ReceiverCurrentByteCount++;
		if (ReceiverCurrentByteCount > 2)
		{
			if (TempDataArray[ReceiverCurrentByteCount - 2] == 0X0D && TempDataArray[ReceiverCurrentByteCount - 1] == 0X0A )  //ÿ�ζ���ȡһ�ν����ж��Ƿ����
			{
				//�ж���һ֡��ǰ������ĸ�����뵽��ͬ�Ĵ������
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
						rp.mode = MODE;//ֱ��Ϊ1 �����Ϊ2

						szBuffer.Set(rp);

						//cout << rp.rtkStatus<< endl;
						/************�洢�ļ�*************/
						//�ڴ洢֮ǰ��������Ҫ��һ��ɸѡ��0.2mһ���㣬��಻��Ҫ����

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
							if (getDist2(p1,p2) >= 0.01 && rp.rtkStatus >= 2)  //2��α����  4���̶���  5��������
							{
								isRecode = true;
							}
						}

					if(isRecode) //ֻ��rtk�ź������������ǲż�¼���� 
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
				ReceiverCurrentByteCount = 0;	//���пո�һ������
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
				rp.speed *= 1.852; //һ�� = һ���� = 1.852km/h
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

double ivSensorGPS::angToRad(double angle_d)  ///����ת����
{
    double Pi = 3.1415926535898;
    double rad;
    rad = angle_d * Pi / 180;
    return rad;
}

double ivSensorGPS::DisBetweenPoints(double latA, double lonA, double latB, double lonB)
{
    //����pi
    const double PI = 3.1415926535897932384626433832795;
    //�������뾶����λΪ��
    const double R = 6371004;
    //��ʽ
    double C = sin(latA) * sin(latB) * cos(lonA - lonB) + cos(latA) * cos(latB);

    double distance = R * acos(C) * PI / 180;

    return distance;
}
 //����ƽ������������
  double ivSensorGPS::getDist2(Point2D p1, Point2D p2)
    {
        return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    }

   double ivSensorGPS::GetL0InDegree(double dLIn)
    {
        //3�������ż����������߾���(d����ʽ)
        //���幫ʽ����ġ����ô������ϵ����任���컪ͳ����ž�������138ҳ
        double L = dLIn;//d.d
        double L_ddd_Style = L;
        double ZoneNumber = (int)((L_ddd_Style - 1.5) / 3.0) + 1;
        double L0 = ZoneNumber * 3.0;//degree
        return L0;
    }
 /************************************************************************/
   /* B: latγ��
    * L: lon����
    * H: height
/************************************************************************/
 Point2D ivSensorGPS::BLH2XYZ(double B, double L, double H)
    {
        double N, E, h;
        double L0 = GetL0InDegree(L);//���ݾ��������������߾���
        Point2D pt2d;
        double a = 6378245.0;            //����뾶  ����6378245
        double F = 298.257223563;        //�������
        double iPI = 0.0174532925199433; //2pi����360�����ڽǶ�ת��

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
        //    //UTMͶӰ
        //    N = 0.9996 * N;
        //    E = 0.9996 * (E - 500000.0) + 500000.0;//Get y
        //}
       /* if (nFlag == 2)
        {*/
            //UTMͶӰ
            N = 0.9999 * N;
            E = 0.9999 * (E - 500000.0) + 250000.0;//Get y
        //}

        //ԭ
        //pt2d.x = N;
        //pt2d.y = E;
        //
        pt2d.x = E;
        pt2d.y = N;
        h = H;

        return pt2d;
    }

 //��������12���汱�ļн�
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

 //��Ҫ���ǽ����ļ���Ȼ�����ݼ��ص�list����
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
	 Point2D nowPosition = BLH2XYZ(nowPoint.lat, nowPoint.lon, 0);//������ǰλ�õľ�γ������ת��Ϊxy����
	 Point2D tempPosition;

	 double distance;
	 double angle;
	 double x;
	 double y;
	 double lasty = -11;

	 int numY=0; //��ǰ�������������һ����������ͳ��
	 bool flag = true;

	 //Ѱ������ĵ㲢����
	 double miniDis = 10000;
	 double Minidis = 0;
	 double PI = 3.141592654;

	 for(unsigned int i = LastIndex; i < roadPoints.size(); ++i) //�����Ҫ����������ֻ��Ҫ��LastIndexΪ0��Ŀǰֻ�������������������Ƿ���������
	 {
		 tempPosition = BLH2XYZ(roadPoints[i].lat, roadPoints[i].lon, 0);
		 distance = getDist2(nowPosition,tempPosition);
		 angle = getAngle(nowPoint.lon, nowPoint.lat,roadPoints[i].lon, roadPoints[i].lat); //���ݾ�γ����������ɵ��������汱�ļнǣ�0-360��
		 angle = angle - nowPoint.azimuth;

		 /************************************************************************/
		 /* ����������ſ��ܺ͵����ŵ�λ���й�ϵ                                 */
		 /************************************************************************/

		 x = distance * sin(angle * PI / 180.0) - AxisOffset;
		 y = distance * cos(angle * PI / 180.0) - HeadOffset;

		 //��yֵ���ж�Ҳ����,��ǰ���������Ҫ������һ�ε� ����Ѱ�ң�������Ѱ��
		 if (x >= -WIDTH_REAL && x <= WIDTH_REAL && y >= 0 && y <= HEIGHT_REAL) //&& y > lasty����������û�л�ͷ·�ˣ�  ��-10m��10m) (0 �� 60)
		 {

			 lasty = y; //����ǰ���yֵ��ֵ��Lasty

			 if(flag) //��һ���ҵ������������������y����
			 {
				 lasty = y; //����ǰ���yֵ��ֵ��Lasty
			 }
			 numY++;
			 LastIndex = i;

			 //�ҵ��ĵڶ������������ĵ㣬��Ϊ��һ�ε������
			 if(flag && numY > 1)
			 {
				 StartIndexRader = LastIndex - 1;//��¼��һ�ε���ʼλ��
				 lasty = -11;
				 flag = false;
			 }
			 //��֤ matrixSend���淢�͵ĵ���׼ȷ�ġ���˵����һ���ҵ�һ�����������ĵ㣬������һ���ǲ��ǻ�������������Ҫ������Ļ���ִ�е�����ķ��Ͳ��֡�
			 if(flag)
			 {
				 continue;
			 }
			 unsigned int n = CAR_POSITION_H - (int)(y * RESOLUTION_REAL);  //100/20   100/10
			 unsigned int m = CAR_POSITION_W + (int)(x * RESOLUTION_REAL);

			 if ((n > 0 && n <= CAR_POSITION_H) && (m > 0 && m <= WIDTH)) //��������ֱ��ʡ��
			 {
				 matrixSendF.point[matrixSendF.Num].x = y;
				 matrixSendF.point[matrixSendF.Num].y = x;
				 matrixSendF.Num++;

				 matrixSend.point[matrixSend.Num].x = n;                     //x �������꣬y�Ǻ�����
				 matrixSend.point[matrixSend.Num].y = m;
				 matrixSend.point[matrixSend.Num].value = roadPoints[i].mode; //���Ե��ʹ��
				 matrixSend.point[matrixSend.Num].U = 0;
				 matrixSend.Num++;

				 if (IsSaveMiniDisFile)
				 {
					 if(distance < miniDis) //Ѱ�ҳ�����켣����ľ����
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
		 else if (numY == 1) //�����һ�ν������if���ڶ���û�н������if����ô�������ָ���Ĭ��ֵ��
		 {
			 lasty = -11;
			 numY = 0;
		 }
		 else if(!flag && numY > 1) // ���ҵ�����Ҫ��ķ�Χ�ڵĵ��ͻȻ��һ���㲻����ʱ�����Ǿ���Ϊ����Ҫ������
		 {
			 break;
		 }
	 }
	 if(StartIndexRader > 5)
	 {
		 LastIndex = StartIndexRader - 5;//��һ�β��ҵ���ʼ��
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
#pragma  region  ȥ�ظ� 			
	 /************************************************************************/
	 /*                                  ȥ�ظ�                              */
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
				 //if (mapProcessor->matrixSend.point[i].value != 128)//����ȣ�����������,ԭ�����Ƕ��پ�Ϊ���١������ȣ���130Ϊ��
				 {
					 matrix.point[k].value = matrixSend.point[i].value;
				 }

				 flag = 1;  //���¿��ٵ������ڣ��ҵ�һ������ƥ�������������������ظ�����ֵ�����˳�����ѭ��
				 break;
			 }
		 }
		 if(flag == 0) //���û���ظ�
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
