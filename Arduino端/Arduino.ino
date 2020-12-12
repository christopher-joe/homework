//引入ESP8266.h头文件，建议使用教程中修改后的文件
#include "ESP8266.h"
#include "SoftwareSerial.h"


#define SSID "LYT的P40"    
#define PASSWORD "aba6ec6b3ae4"
#define HOST_NAME "api.heclouds.com"  
#define DEVICE_ID "646522489"    
#define HOST_PORT (80)        
String APIKey = "4DsuCJwlaJLKxFtp6m95g9We16c=";

#define INTERVAL_SENSOR 5000 //定义传感器采样及发送时间间隔

int a,b,sum;

//定义ESP8266软串口
SoftwareSerial mySerial(3, 2);
ESP8266 wifi(mySerial);



void setup()
{
  mySerial.begin(115200); //初始化软串口
  Serial.begin(9600);     //初始化串口
  Serial.print("setup begin\r\n");

  //以下为ESP8266初始化的代码
  Serial.print("FW Version: ");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStation()) {
    Serial.print("to station ok\r\n");
  } else {
    Serial.print("to station err\r\n");
  }

  //ESP8266接入WIFI
  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }


  mySerial.println("AT+UART_CUR=9600,8,1,0,0");
  mySerial.begin(9600);
  Serial.println("setup end\r\n");
}

unsigned long net_time1 = millis(); //数据上传服务器时间
void loop(){


//以下为人流统计部分代码
int n=analogRead(A1);//a1接口为零时有人
if(n==0)
a=a+1;
 
else 
a=a;
int m=analogRead(A2);
if(m==0)
b=b+1;

else 
b=b; 
if(m==0||n==0)
{sum=a-b;
Serial.println(sum); }
delay(800);



  if (net_time1 > millis())
    net_time1 = millis();

    Serial.println(sum);

    Serial.println("");

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
      Serial.print("create tcp ok\r\n");
      char buf[10];


      //拼接POST请求字符串
      String jsonToSend = "{\"number\":";
      dtostrf(sum, 1, 2, buf);
      jsonToSend += "\"" + String(buf) + "\"";
      jsonToSend += "}";
      
      String postString = "POST /devices/";
      postString += DEVICE_ID;
      postString += "/datapoints?type=3 HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
      postString += "Content-Length:";

      postString += jsonToSend.length();
      
      postString += "\r\n";
      postString += "\r\n";
      postString += jsonToSend;
      postString += "\r\n";
      postString += "\r\n";
      postString += "\r\n";

      const char *postArray = postString.c_str(); //将str转化为char数组

      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray)); //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
      Serial.println("send success");
      if (wifi.releaseTCP()) { //释放TCP连接
        Serial.print("release tcp ok\r\n");
      } else {
        Serial.print("release tcp err\r\n");
      }
      postArray = NULL; //清空数组，等待下次传输数据
    } else {
      Serial.print("create tcp err\r\n");
    }

    Serial.println("");

    net_time1 = millis();
  }
