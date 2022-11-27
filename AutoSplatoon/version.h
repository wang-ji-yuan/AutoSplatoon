#ifndef VERSION_H
#define VERSION_H

#define SOFTWARE_NAME       "AutoSplatoon"
#define MAIN_VERSION        0.1.6(beta)

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
ver0.1.6
1.增加了把当前的界面保存为320x120图片的功能,如果需要后期修改(添加文字之类的比较方便).
2.最天晚上在电视上用工具涂了一次，发现我之前的算法有点问题（向右涂的时候最后一格没有涂到,SWITCH屏太小，之前没注意到这个问题)。这个版本修复了这个问题.




ver0.1.5
重要更新！
1.不限制图片格式，不限制图片大小；
2，图片可以移动，缩放；
3，可以自动自动把图片模拟成灰度效果（本次更新中最重要的功能）；
4,现在可以直接看到反色后的效果



ver0.1.4
1.增加了软件版本显示,直接修改version.h
2.修复0.1.3时间估算不准确的问题
ver0.1.3
1.增加了黑白反色的功能,使用后黑白色会互换;最终生成的效果会相反。根据内容的不同，画图的时间会受影响；
2.模拟画点的过程,需要画点的地方会显示出来,颜色可以设置；只影响PC上的显示效果，不影响实际输出；
3.增加了耗时估算的功能，每次上传文件或者修改开始位置后会自动估算耗时；
4.简单优化了画图算法，图像两边有较多空白时能较大的减小画图的时间。尝试一下反色也许可以节约一些时间；
5.增加了画图结束后保存的功能（在 jiangotto的项目上看到的用户请求，我手里没有SWITCH lite,不知道是不是按键不一样。普通SWITCH上是“-”按键；



*/
