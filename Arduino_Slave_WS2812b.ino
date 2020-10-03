#include "FastLED.h"            // 灯光需要用到的库文件
#include "avr/pgmspace.h"       // 数据写入FLASH与从FLASH读取数据需要用到的库文件 
#include "math.h"               // 进行数学运算需要用到的库文件
#include <SPI.h>                // 与主机通讯需要用到的库文件
 
#define NUM_LEDS 512            // 定义LED灯珠数量
#define LED_DT 9                // 定义Arduino输出控制信号引脚
#define LED_TYPE WS2812         // 定义LED灯带型号
#define COLOR_ORDER GRB         // 定义RGB灯珠中红色、绿色、蓝色LED的排列顺序
CRGB leds[NUM_LEDS];            // 建立光带leds
                                // 以上是灯带的初始化

//uint8_t max_bright = 20;      // LED亮度控制变量，可使用数值为 0 ～ 255， 数值越大则光带亮度越高

volatile boolean received;
volatile byte Slavereceived,Slavesend;  // 定义与主机的通讯


int second=0;
int i=0;
const int a[16][32] PROGMEM =     //定义写入FLASH函数，最大限度缩减RAM的占用内存，防止程序崩溃甚至写入失败，后面会用指针来调用存入FLASH的内容，转入RAM进行loop
{
  {248 ,247 ,232 ,231 ,216 ,215 ,200 ,199 ,184 ,183 ,168 ,167 ,152 ,151 ,136 ,135 ,120 ,119 ,104 ,103 ,88  ,87  ,72  ,71  ,56  ,55  ,40  ,39  ,24  ,23  ,8 ,7},
  
  {249 ,246 ,233 ,230 ,217 ,214 ,201 ,198 ,185 ,182 ,169 ,166 ,153 ,150 ,137 ,134 ,121 ,118 ,105 ,102 ,89  ,86  ,73  ,70  ,57  ,54  ,41  ,38  ,25  ,22  ,9 ,6},
  
  {250 ,245 ,234 ,229 ,218 ,213 ,202 ,197 ,186 ,181 ,170 ,165 ,154 ,149 ,138 ,133 ,122 ,117 ,106 ,101 ,90  ,85  ,74  ,69  ,58  ,53  ,42  ,37  ,26  ,21  ,10  ,5},
  
  {251 ,244 ,235 ,228 ,219 ,212 ,203 ,196 ,187 ,180 ,171 ,164 ,155 ,148 ,139 ,132 ,123 ,116 ,107 ,100 ,91  ,84  ,75  ,68  ,59  ,52  ,43  ,36  ,27  ,20  ,11  ,4},
  
  {252 ,243 ,236 ,227 ,220 ,211 ,204 ,195 ,188 ,179 ,172 ,163 ,156 ,147 ,140 ,131 ,124 ,115 ,108 ,99  ,92  ,83  ,76  ,67  ,60  ,51  ,44  ,35  ,28  ,19  ,12  ,3},
  
  {253 ,242 ,237 ,226 ,221 ,210 ,205 ,194 ,189 ,178 ,173 ,162 ,157 ,146 ,141 ,130 ,125 ,114 ,109 ,98  ,93  ,82  ,77  ,66  ,61  ,50  ,45  ,34  ,29  ,18  ,13  ,2},
  
  {254 ,241 ,238 ,225 ,222 ,209 ,206 ,193 ,190 ,177 ,174 ,161 ,158 ,145 ,142 ,129 ,126 ,113 ,110 ,97  ,94  ,81  ,78  ,65  ,62  ,49  ,46  ,33  ,30  ,17  ,14  ,1},
  
  {255 ,240 ,239 ,224 ,223 ,208 ,207 ,192 ,191 ,176 ,175 ,160 ,159 ,144 ,143 ,128 ,127 ,112 ,111 ,96  ,95  ,80  ,79  ,64  ,63  ,48  ,47  ,32  ,31  ,16  ,15  ,0},
  
  {256 ,271 ,272 ,287 ,288 ,303 ,304 ,319 ,320 ,335 ,336 ,351 ,352 ,367 ,368 ,383 ,384 ,399 ,400 ,415 ,416 ,431 ,432 ,447 ,448 ,463 ,464 ,479 ,480 ,495 ,496 ,511},
  
  {257 ,270 ,273 ,286 ,289 ,302 ,305 ,318 ,321 ,334 ,337 ,350 ,353 ,366 ,369 ,382 ,385 ,398 ,401 ,414 ,417 ,430 ,433 ,446 ,449 ,462 ,465 ,478 ,481 ,494 ,497 ,510},
  
  {258 ,269 ,274 ,285 ,290 ,301 ,306 ,317 ,322 ,333 ,338 ,349 ,354 ,365 ,370 ,381 ,386 ,397 ,402 ,413 ,418 ,429 ,434 ,445 ,450 ,461 ,466 ,477 ,482 ,493 ,498 ,509},
  
  {259 ,268 ,275 ,284 ,291 ,300 ,307 ,316 ,323 ,332 ,339 ,348 ,355 ,364 ,371 ,380 ,387 ,396 ,403 ,412 ,419 ,428 ,435 ,444 ,451 ,460 ,467 ,476 ,483 ,492 ,499 ,508},
  
  {260 ,267 ,276 ,283 ,292 ,299 ,308 ,315 ,324 ,331 ,340 ,347 ,356 ,363 ,372 ,379 ,388 ,395 ,404 ,411 ,420 ,427 ,436 ,443 ,452 ,459 ,468 ,475 ,484 ,491 ,500 ,507},
  
  {261 ,266 ,277 ,282 ,293 ,298 ,309 ,314 ,325 ,330 ,341 ,346 ,357 ,362 ,373 ,378 ,389 ,394 ,405 ,410 ,421 ,426 ,437 ,442 ,453 ,458 ,469 ,474 ,485 ,490 ,501 ,506},
  
  {262 ,265 ,278 ,281 ,294 ,297 ,310 ,313 ,326 ,329 ,342 ,345 ,358 ,361 ,374 ,377 ,390 ,393 ,406 ,409 ,422 ,425 ,438 ,441 ,454 ,457 ,470 ,473 ,486 ,489 ,502 ,505},
  
  {263 ,264 ,279 ,280 ,295 ,296 ,311 ,312 ,327 ,328 ,343 ,344 ,359 ,360 ,375 ,376 ,391 ,392 ,407 ,408 ,423 ,424 ,439 ,440 ,455 ,456 ,471 ,472 ,487 ,488 ,503 ,504}
  }; 
  //定义二维数组，将线性的WS2812b灯带组重新编组，并为其定义二维坐标，以便于后期定位编程，尤其是ripple涟漪函数，需要上百组迭代精确定位
  








void setup() { 
  
  
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);  // 初始化光带
  uint8_t max_bright = 60; 
 FastLED.setBrightness(max_bright);                             // 设置光带亮度
  Serial.begin(115200);
                                                                // 以上是灯带的setup
  pinMode(MISO,OUTPUT);
  SPCR|=_BV(SPE);
  received = false;
  SPI.attachInterrupt();                                        //初始化与主机的通讯
}

 ISR (SPI_STC_vect)                        // 定义中断进程功能 
{
  Slavereceived = SPDR;                    // 从SPDR中读取主机写入的数据
  received = true;                         // 开启数据收入功能
}








void loop () {
  // Serial.print("Slavereceived");
  Serial.println(Slavereceived);

   if(Slavereceived==3)
     {
      wave(20);                  //向左边掠过波浪的效果
     }


     
    if(Slavereceived==4)
     {
      wave_reverse(20);         //向反方向掠过波浪的效果
     }



     
     if(Slavereceived==1)           // 如果受到主机发来的"FORWARD"信息，即手向下压的动作
     {
      int s=0,t=0;  
    for(s=0;s<16;s=s+5)
    {
      ripple(7,15,s,0,0,255);       // 第一圈涟漪
      ripple(7,15,s+1,0,0,255);     // 第一圈半涟漪，用于补充上一圈涟漪
      ripple(7,15,s+2,0,0,255);     // 第二圈涟漪
      ripple(7,15,s+3,0,0,255);     // 第二圈半涟漪，用于补充上一圈涟漪
    }
     }    //涟漪从某个点泛开的效果：我们采用单行遍历检索的方式，确定发光半径，由于机器检索速度较慢，导致动效不够连贯，所以采用叠加的方式来进一步拟合涟漪动效


     
     if(Slavereceived==2)           // 如果受到主机发来的"BACKWARD"的信号，即手向上抬起的动作
     {
      explode();                    // 光幕亮起并渐灭，超新星快速爆发的感觉
      starry_sky();                 // 星子在全局随机亮起，光幕短暂亮起后碎成一颗颗星子，营造一种梦幻的氛围
     }          //用渐强光来刷新现有状态，营造一种explode的效果，并把强光幕碎裂成闪烁的星子，目的在于为使用者呈现夏夜躺在草地看星空的感受




     
     if(Slavereceived==5)
     {
      star_rise();         //星星升起的效果
     }



     
     if(Slavereceived==6)
     {
      star_fall();        //渐暗的背景下，流星随机划下的效果。为使用者营造一种夜色熏熏，浅草浮动，月影婆娑，流星划过天际，辉光拂动鬓角的感受。
     }


     if(Slavereceived==7)
     {
      breath();          //初始状态的呼吸灯效果
     }

     Slavesend=123;                             
     SPDR = Slavesend;     //从机进行信息的收入


} 








void wave(int d)
{
  
  int b,c;
  for (c=0;c<32;c++)
  {
   if(c<32){
    leds[pgm_read_word(&a[0][c])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[1][c+1])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[2][c+1])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[3][c+2])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[4][c+2])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[5][c+3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[6][c+3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[7][c+3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[8][c+3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[9][c+3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[10][c+3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[11][c+2])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[12][c+2])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[13][c+1])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[14][c+1])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[15][c])]=CHSV(0,0,255);
    FastLED.show();
    delay(40);

    fadeToBlackBy( leds, NUM_LEDS, d);
   }
   else break;
  }
  }                                                    //波浪函数的定义



  void wave_reverse(int d)
{
  
  int b,c;
  for (c=31;c>=0;c--)
  {
   if(c>=0){
    leds[pgm_read_word(&a[0][c])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[1][c-1])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[2][c-1])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[3][c-2])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[4][c-2])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[5][c-3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[6][c-3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[7][c-3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[8][c-3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[9][c-3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[10][c-3])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[11][c-2])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[12][c-2])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[13][c-1])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[14][c-1])]=CHSV(0,0,255);
    leds[pgm_read_word(&a[15][c])]=CHSV(0,0,255);
    FastLED.show();
    delay(40);

    fadeToBlackBy( leds, NUM_LEDS, d);
   }
   else break;
  } 
  }                                                   //反向波浪函数的定义





void breath()
{
  
  int C;
  for(C=10;C<40;C++)
  {

    uint8_t max_bright = (C); 
   FastLED.setBrightness(max_bright);
   fill_gradient(leds, 127, CHSV(0,0,255) , 0, CHSV(0,0,50), LONGEST_HUES);
   fill_gradient(leds, 128, CHSV(0,0,255) , 255, CHSV(0,0,50), LONGEST_HUES);
   fill_gradient(leds, 384, CHSV(0,0,255) , 511, CHSV(0,0,50), LONGEST_HUES);
   fill_gradient(leds, 383, CHSV(0,0,255) , 256, CHSV(0,0,50), LONGEST_HUES);
   FastLED.show();
   delay(50);
   //fadeToBlackBy( leds, NUM_LEDS, 20);
    //Serial.println(C);
  }
for(C=40;C>10;C--)
  {

    uint8_t max_bright = (C); 
   FastLED.setBrightness(max_bright);
   fill_gradient(leds, 127, CHSV(0,0,255) , 0, CHSV(0,0,50), LONGEST_HUES);
   fill_gradient(leds, 128, CHSV(0,0,255) , 255, CHSV(0,0,50), LONGEST_HUES);
   fill_gradient(leds, 384, CHSV(0,0,255) , 511, CHSV(0,0,50), LONGEST_HUES);
   fill_gradient(leds, 383, CHSV(0,0,255) , 256, CHSV(0,0,50), LONGEST_HUES);
   FastLED.show();
   //fadeToBlackBy( leds, NUM_LEDS, 20);
    //Serial.println(C);
    delay(50);
  }
  
  }                                                                               //呼吸灯的定义







void explode()   
{
  
  int C;
  for(C=0;C<80;C++)
  {
    if(C<80){
    uint8_t max_bright = (C); 
   FastLED.setBrightness(max_bright);
   fill_gradient(leds, 127, CHSV(0,0,255) , 0, CHSV(0,0,50), LONGEST_HUES);
   fill_gradient(leds, 128, CHSV(0,0,255) , 255, CHSV(0,0,50), LONGEST_HUES);
   fill_gradient(leds, 384, CHSV(0,0,255) , 511, CHSV(0,0,50), LONGEST_HUES);
   fill_gradient(leds, 383, CHSV(0,0,255) , 256, CHSV(0,0,50), LONGEST_HUES);
   FastLED.show();
   fadeToBlackBy( leds, NUM_LEDS, 20);
    //Serial.println(C);

    }
  
  else break;
  }
  }                                                                              //高亮光幕+渐暗效果的定义
void starry_sky()
{

   int A,B,c;

   for(c=0;c<100;c++){
    if(c<100){
   A=random(0,15);
   B=random(0,31);
   
   leds[pgm_read_word(&(a[A][B]))]=CHSV(0,0,255);
   delay(50);
   fadeToBlackBy( leds, NUM_LEDS, 10);

    FastLED.show();

    }
    else break;
   } 
  }                                                                              // 星空效果的定义
 


void ripple(int x,int y,int r,int s,int t,int u)       //定义涟漪函数，六个变量分别是圆心的横、纵坐标、圆半径、光环的三相HSV值

{                                                      //由于涟漪动效较为复杂，我们把动效拆成半径可变的光环，再通过loop函数进行涟漪动效的进一步实现
  int m,n;
  int R;

  for(m=0;m<16;m++)
  {
    for(n=0;n<32;n++)
    {
  
  R=sqrt(abs((m-x))*abs((m-x))+abs((n-y))*abs((n-y)));  //检索灯板上所有灯珠，以流水动效来点亮距离圆心某距离的所有点（取整）
  if(R==r)
  {
    leds[pgm_read_word(&a[m][n])]=CHSV(s,t,u);
    //leds[a[m][n]]=CHSV(0,0,255);
    //Serial.println(m);
    //Serial.println(n);
    FastLED.show();
    fadeToBlackBy( leds, NUM_LEDS, 10);
    }
   }
  }
}
  






void star_rise()
{
  int b,c,d,e;
  b=random(0,31);
  d=random(70,100);
  e=random(155,255);
  for(c=0;c<=15;c++)
  {
    leds[pgm_read_word(&a[c][b])]=CHSV(0,0,e);
    FastLED.show();
    fadeToBlackBy( leds, NUM_LEDS, d);
    
    }
  }                                                                // 星子随机上升效果的定义

  void star_fall()
{
  int b,c,d,e;
  b=random(0,31);
  d=random(70,100);
  e=random(155,255);
  for(c=15;c>=0;c--)
  {
    leds[pgm_read_word(&a[c][b])]=CHSV(0,0,e);
    FastLED.show();
    fadeToBlackBy( leds, NUM_LEDS, d);
    
    }
  }                                                              // 星子随机落下效果的定义


  
