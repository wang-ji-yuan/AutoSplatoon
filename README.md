<!--
 * @Author: wudy
 * @Date: 2022-11-21 12:02:49
 * @LastEditTime: 2022-11-23 15:37:11
 * @LastEditors: wudy
 * @Description: 
 *  
-->
# AutoSplatoon

基于[ClubchatGames](https://github.com/nullstalgia/ClubchatGames)

---
**软件界面：**

![UI](image/AutoSplatoon_User_Interface.png)

---
**实机效果**

![实机效果](image/Performance_on_Switch.jpg)

---
**支持硬件：**

ESP32系列，e.g. ESP32 WROOM模组、ESP32 WROVER模组、ESP32 PICO V3芯片、ESP32 PICO D4芯片...

---
**使用教程：**

详见B站[【斯普拉遁】广场涂鸦自动化工具！保姆级教程](https://www.bilibili.com/video/BV1va411R7TJ?vd_source=08b359f4e68b47a7ff089bcfa5caa191)

![视频封面](image/Video_Cover.png)

---
**TODO**

- [x] 支持用户自定义绘图速度
- [ ] 内置图像灰度处理，无需使用Photoshop事先处理
- [x] 根据画面黑白像素个数优化绘图速度
- [x] 支持从指定位置开始绘图,需要同主机端的画笔位置相同,有点不好操作


更新过程：
ver0.1.4
1.增加了软件版本显示,直接修改version.h
2.修复0.1.3时间估算不准确的问题
ver0.1.3
2.增加了黑白反色的功能,使用后黑白色会互换;最终生成的效果会相反。根据内容的不同，画图的时间会受影响；
3.模拟画点的过程,需要画点的地方会显示出来,颜色可以设置；只影响PC上的显示效果，不影响实际输出；
4.增加了耗时估算的功能，每次上传文件或者修改开始位置后会自动估算耗时；
5.简单优化了画图算法，图像两边有较多空白时能较大的减小画图的时间。尝试一下反色也许可以节约一些时间；
6.增加了画图结束后保存的功能（在 jiangotto的项目上看到的用户请求，我手里没有SWITCH lite,不知道是不是按键不一样。普通SWITCH上是“-”按键；

