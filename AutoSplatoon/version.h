#ifndef VERSION_H
#define VERSION_H

#define SOFTWARE_NAME       "AutoSplatoon"
#define MAIN_VERSION        0.1.4(beta)

#define _STR_(s)            #s
#define __STR(s)            _STR_(s)

#define BUILD_VERSION       _STR_(87564M)
#define SOFTWARE_VERSION    __STR(MAIN_VERSION)

#if QT_NO_DEBUG
#define SOFTWARE_TITLE      (SOFTWARE_NAME " V" SOFTWARE_VERSION)
#else
#define SOFTWARE_TITLE      (SOFTWARE_NAME " V" SOFTWARE_VERSION \
                            " (Build " BUILD_VERSION "-Debug)")
#endif
#endif // VERSION_H
/*
更新过程：
ver0.1.4
1.增加了软件版本显示,直接修改version.h
2.增加了剩余时间显示的功能；
3.优化了耗时时间的刷新时机；
ver0.1.3
2.增加了黑白反色的功能,使用后黑白色会互换;最终生成的效果会相反。根据内容的不同，画图的时间会受影响；
3.模拟画点的过程,需要画点的地方会显示出来,颜色可以设置；只影响PC上的显示效果，不影响实际输出；
4.增加了耗时估算的功能，每次上传文件或者修改开始位置后会自动估算耗时；
5.简单优化了画图算法，图像两边有较多空白时能较大的减小画图的时间。尝试一下反色也许可以节约一些时间；
6.增加了画图结束后保存的功能（在 jiangotto的项目上看到的用户请求，我手里没有SWITCH lite,不知道是不是按键不一样。普通SWITCH上是“-”按键；



*/