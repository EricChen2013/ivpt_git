#ifndef NODE_EXAMPLE_TALKER_H
#define NODE_EXAMPLE_TALKER_H

// ROS includes.
#include "ros/ros.h"
#include "ros/time.h"
//lib
#include "serial/serial.h"
// Custom message includes. Auto-generated from msg/ directory.
#include "ivsensorgps/gpsmsg.h"
using namespace std;
typedef struct sGpsPoint
{
  double lon;
  double lat;
  unsigned char mode;
  double heading;
  double velocity;
  unsigned char status;
  unsigned char satenum;
}sGpsPoint;
 
class gpsparse
{
  public:
  //! Constructor.
  gpsparse(ros::NodeHandle nh);
  ~gpsparse();

  private:
  //! Timer callback for publishing message.
  void timerCallback(const ros::TimerEvent& event);
  // void timerCallback2(const ros::TimerEvent& event);
  void naviColKernel();
  // void naviDealKernel();
  void receiveData(unsigned char* str, int len);
  void parseBestPosa();
  void parseGptra();
  void parseGpvtg();
  void parseGpgga();
  void parseGprmc();
  void parseGphpr();
  void publishMsg();
  void parseGpybm();
  private:
  //! The timer variable used to go to callback function at specified rate.
  ros::Timer timer_;
  ros::Timer timer2_;
  //! Message publisher.
  ros::Publisher pub_;
  //
  serial::Serial ser;
  //gps object
  sGpsPoint rp;
  //for gps data parse
  unsigned char TempDataArray[350];
  int ReceiverCurrentByteCount;      
  int sendCount;
  int gpsDeviceName; //0-sinan 1-lianshi 2-qianxun
};

#endif // NODE_EXAMPLE_TALKER_H
