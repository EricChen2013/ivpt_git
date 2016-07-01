#include "gpsparse.h" 

 gpsparse::gpsparse(ros::NodeHandle nh) {
   //data initialize 
   memset(&rp,0,sizeof(rp));
   gpsDeviceName = "lianshi";
   ReceiverCurrentByteCount = 0;
   sendCount = 0;
   //Create a publisher and name the topic.
   pub_ = nh.advertise<ivsensorgps::gpsmsg>("ivsensorgps", 1000);
   // Create timer.
   int rate = 10;
   timer_ = nh.createTimer(ros::Duration(1 / rate), &gpsparse::timerCallback, this);
   //timer2_ = nh.createTimer(ros::Duration(1 / rate), &gpsparse::timerCallback2, this);
   //serial port initialize and open
   /*
   try
   {
        ser.setPort("/dev/ttyACM0");
        ser.setBaudrate(9600);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
   }
   catch (serial::IOException& e)
   {
        ROS_ERROR_STREAM("Unable to open port ");
   }
   if(ser.isOpen())
   {
       ROS_INFO_STREAM("Serial Port initialized");
   }
   else
   {
       ROS_ERROR_STREAM("Serial Port failed");
   }
   */
 }
 //
 gpsparse::~gpsparse()
 {

 }
 //
 void gpsparse::timerCallback(const ros::TimerEvent& event)
 {
         //naviColKernel();
   publishMsg();   
 }
 //
 void gpsparse::naviColKernel()
{
   std::string recvstr;
   recvstr = ser.read(ser.available());
   std::cout<<recvstr<<std::endl;
   //here read serialport data
   const int bufsize = 1000;
   unsigned char revbuf[bufsize];
   memset(&revbuf,0,sizeof(revbuf));
   int len = ser.read(revbuf, bufsize); 
   if(len > 0 && len < bufsize)
   {
     receiveData(revbuf,len);
   }
} 
//
 void gpsparse::publishMsg()
{
   ivsensorgps::gpsmsg msg;
   msg.lon = rp.lon;
   msg.lat = rp.lat;
   msg.mode = rp.mode;
   msg.heading = rp.heading;
   msg.velocity = rp.velocity;
   msg.status = rp.status;
   msg.satenum = rp.satenum;  
   pub_.publish(msg);
}
//
void gpsparse::receiveData(unsigned char* str, int len)
{
  for (int i = 0; i < len; ++i)
  {
           TempDataArray[ReceiverCurrentByteCount] = str[i];
           ReceiverCurrentByteCount++;
           if (ReceiverCurrentByteCount > 2)
          {

             if (TempDataArray[ReceiverCurrentByteCount - 2] == 0X0D && TempDataArray[ReceiverCurrentByteCount - 1] == 0X0A )  
             {
                    if (ReceiverCurrentByteCount > 6)
                    {
                             if (!gpsDeviceName.compare("sinan"))
                             {
                                if (TempDataArray[0] == '#' && TempDataArray[1] == 'B' && TempDataArray[2] == 'E'&& TempDataArray[3] == 'S')
                                {
                                   parseBestPosa();
                                   sendCount = (sendCount + 1) % 3;
                                }
                                else if (TempDataArray[0] == '$' && TempDataArray[1] == 'G' && TempDataArray[2] == 'P'&& TempDataArray[3] == 'T')
                                {
                                   parseGptra();
                                   sendCount = (sendCount + 1) % 3;
                                }
                                else if (TempDataArray[0] == '$' && TempDataArray[1] == 'G' && TempDataArray[2] == 'P'&& TempDataArray[3] == 'V')
                                {
                                  parseGpvtg();
                                  sendCount = (sendCount + 1) % 3;
                                }
                             } 
                             else if(!gpsDeviceName.compare("lianshi"))
                             {
                                if (TempDataArray[0] == '$' && TempDataArray[1] == 'G' && TempDataArray[2] == 'P'&& TempDataArray[3] == 'G')
                                {
                                  parseGpgga();
                                  sendCount = (sendCount + 1) % 3;
                                }
                                else if (TempDataArray[0] == '$' && TempDataArray[1] == 'G' && TempDataArray[2] == 'P'&& TempDataArray[3] == 'R')
                                {
                                  parseGprmc();
                                  sendCount = (sendCount + 1) % 3;
                                }
                                else if (TempDataArray[0] == '$' && TempDataArray[1] == 'P' && TempDataArray[2] == 'S'&& TempDataArray[3] == 'A')
                                {
                                  parseGphpr();
                                  sendCount = (sendCount + 1) % 3;
                                }
                             }      
                            
                    }
                 ReceiverCurrentByteCount = 0; //¿ÕÐÐ¿ÕžñÒ»¶šÇåÁã
              }
         }   
     }
}
//
void gpsparse::parseBestPosa()
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
     case 'F': rp.status = 2;          break;             //"FIXEDPOS"
     case 'S': 
      if (temp[1] == 'I')
      {
       rp.status = 3; 
      } 
      else
      {
       rp.status = 8; 
      }
      break;  //"SINGLE"  "SUPER WIDE-LANE"
     case 'P':rp.status = 4;           break;  //"PSRDIFF"
     case 'N':
      if (temp[1] == 'O')
      {
       rp.status = 1;
      }
      else if (temp[7] == 'F')
      {
       rp.status = 5; 
      }else if (temp[7] == 'I')
      {
       rp.status = 7; 
      }
      break;  //   "NONE"  "NARROW_FLOAT"  "NARRAW_INT"
     case 'W':rp.status = 6;       break;  //"WIDE_INT"
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
//
void gpsparse::parseGphpr()
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
    stream >> rp.heading;  
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
//
void gpsparse::parseGptra()
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
    case 3:  stream >> rp.heading;   stream.str(""); temp=""; break;
    case 6:  stream >> rp.status; stream.str(""); temp=""; break;
    case 7:  stream >> rp.satenum;   stream.str(""); temp=""; break;
    default: stream.str(""); temp=""; break;
   }
  }
  else 
  {
   temp += TempDataArray[i];
  } 
 }
}
//
void gpsparse::parseGprmc()
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
    stream >> rp.velocity;  
    rp.velocity *= 1.852; //Ò»œÚ = Ò»º£Àï = 1.852km/h
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
//
void gpsparse::parseGpvtg()
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
   case 8:  stream >> rp.velocity;  stream.str(""); temp=""; break;
   default: stream.str(""); temp=""; break;
   }
  }
  else 
  {
   temp += TempDataArray[i];
  } 
 }
}
//
void gpsparse::parseGpgga()
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
    stream >> rp.status;
    stream.str(""); temp=""; break;
   case 8:
    stream >> rp.satenum;
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




