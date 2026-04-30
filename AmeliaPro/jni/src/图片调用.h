#ifndef 手持调用_H
#define 手持调用_H

#include "VulkanUtils.h"

#include <map>

// 包含所有图片资源
#include "手持图片/101001.h"
#include "手持图片/101002.h"
#include "手持图片/101003.h"
#include "手持图片/101004.h"
#include "手持图片/101005.h"
#include "手持图片/101006.h"
#include "手持图片/101007.h"
#include "手持图片/101008.h"
#include "手持图片/101009.h"
#include "手持图片/101010.h"
#include "手持图片/101011.h"
#include "手持图片/101012.h"
#include "手持图片/102001.h"
#include "手持图片/102002.h"
#include "手持图片/102003.h"
#include "手持图片/102004.h"
#include "手持图片/102005.h"
#include "手持图片/102007.h"
#include "手持图片/102105.h"
#include "手持图片/103001.h"
#include "手持图片/103002.h"
#include "手持图片/103003.h"
#include "手持图片/103004.h"
#include "手持图片/103005.h"
#include "手持图片/103006.h"
#include "手持图片/103007.h"
#include "手持图片/103008.h"
#include "手持图片/103009.h"
#include "手持图片/103010.h"
#include "手持图片/103011.h"
#include "手持图片/103012.h"
#include "手持图片/103013.h"
#include "手持图片/103014.h"
#include "手持图片/103015.h"
#include "手持图片/103100.h"
#include "手持图片/103901.h"
#include "手持图片/103902.h"
#include "手持图片/103903.h"
#include "手持图片/104001.h"
#include "手持图片/104002.h"
#include "手持图片/104003.h"
#include "手持图片/104004.h"
#include "手持图片/104100.h"
#include "手持图片/105001.h"
#include "手持图片/105002.h"
#include "手持图片/105010.h"
#include "手持图片/106001.h"
#include "手持图片/106002.h"
#include "手持图片/106003.h"
#include "手持图片/106004.h"
#include "手持图片/106005.h"
#include "手持图片/106006.h"
#include "手持图片/106007.h"
#include "手持图片/106008.h"
#include "手持图片/106010.h"
#include "手持图片/107001.h"
#include "手持图片/107007.h"
#include "手持图片/108001.h"
#include "手持图片/108002.h"
#include "手持图片/108003.h"
#include "手持图片/108004.h"
#include "手持图片/602001.h"
#include "手持图片/602002.h"
#include "手持图片/602003.h"
#include "手持图片/602004.h"
#include "手持图片/101013.h"
#include "手持图片/102008.h"
#include "手持图片/106011.h"
#include "手持图片/107006.h"
#include "手持图片/107008.h"
#include "手持图片/105015.h"
#include "手持图片/真人.h"
#include "手持图片/人机.h"
#include "手持图片/拳头.h"
#include "手持图片/盒子.h"
#include "手持图片/自瞄按钮.h"
#include "手持图片/手雷.h"
#include "手持图片/烟雾弹.h"
#include "手持图片/震撼弹.h"
#include "手持图片/燃烧瓶.h"
#include "手持图片/铝热弹.h"

extern std::map<int, MyTextureData> 手持图片;

inline void 加载内存图片()
{
        手持图片[0] = createTexture_ALL_FromMem(picture_拳头_png, sizeof(picture_拳头_png));
        手持图片[3] = createTexture_ALL_FromMem(FunnelIcon4, sizeof(FunnelIcon4));
        手持图片[4] = createTexture_ALL_FromMem(FunnelIcon5, sizeof(FunnelIcon5));
    
        手持图片[101013] = createTexture_ALL_FromMem(picture_101013_png, sizeof(picture_101013_png));
        手持图片[102008] = createTexture_ALL_FromMem(picture_102008_png, sizeof(picture_102008_png));
        手持图片[106011] = createTexture_ALL_FromMem(picture_106011_png, sizeof(picture_106011_png));
        手持图片[107006] = createTexture_ALL_FromMem(picture_107006_png, sizeof(picture_107006_png));
        手持图片[107008] = createTexture_ALL_FromMem(picture_107008_png, sizeof(picture_107008_png));
        手持图片[101001] = createTexture_ALL_FromMem(picture_101001_png, sizeof(picture_101001_png));
        手持图片[101002] = createTexture_ALL_FromMem(picture_101002_png, sizeof(picture_101002_png));
        手持图片[101003] = createTexture_ALL_FromMem(picture_101003_png, sizeof(picture_101003_png));
        手持图片[101004] = createTexture_ALL_FromMem(picture_101004_png, sizeof(picture_101004_png));
        手持图片[101005] = createTexture_ALL_FromMem(picture_101005_png, sizeof(picture_101005_png));
        手持图片[101006] = createTexture_ALL_FromMem(picture_101006_png, sizeof(picture_101006_png));
        手持图片[101007] = createTexture_ALL_FromMem(picture_101007_png, sizeof(picture_101007_png));
        手持图片[101008] = createTexture_ALL_FromMem(picture_101008_png, sizeof(picture_101008_png));
        手持图片[101009] = createTexture_ALL_FromMem(picture_101009_png, sizeof(picture_101009_png));
        手持图片[101010] = createTexture_ALL_FromMem(picture_101010_png, sizeof(picture_101010_png));
        手持图片[101011] = createTexture_ALL_FromMem(picture_101011_png, sizeof(picture_101011_png));
        手持图片[101012] = createTexture_ALL_FromMem(picture_101012_png, sizeof(picture_101012_png));
        手持图片[102001] = createTexture_ALL_FromMem(picture_102001_png, sizeof(picture_102001_png));
        手持图片[102002] = createTexture_ALL_FromMem(picture_102002_png, sizeof(picture_102002_png));
        手持图片[102003] = createTexture_ALL_FromMem(picture_102003_png, sizeof(picture_102003_png));
        手持图片[102004] = createTexture_ALL_FromMem(picture_102004_png, sizeof(picture_102004_png));
        手持图片[102005] = createTexture_ALL_FromMem(picture_102005_png, sizeof(picture_102005_png));
        手持图片[102007] = createTexture_ALL_FromMem(picture_102007_png, sizeof(picture_102007_png));
        手持图片[102105] = createTexture_ALL_FromMem(picture_102105_png, sizeof(picture_102105_png));
        手持图片[103001] = createTexture_ALL_FromMem(picture_103001_png, sizeof(picture_103001_png));
        手持图片[103002] = createTexture_ALL_FromMem(picture_103002_png, sizeof(picture_103002_png));
        手持图片[103003] = createTexture_ALL_FromMem(picture_103003_png, sizeof(picture_103003_png));
        手持图片[103004] = createTexture_ALL_FromMem(picture_103004_png, sizeof(picture_103004_png));
        手持图片[103005] = createTexture_ALL_FromMem(picture_103005_png, sizeof(picture_103005_png));
        手持图片[103006] = createTexture_ALL_FromMem(picture_103006_png, sizeof(picture_103006_png));
        手持图片[103007] = createTexture_ALL_FromMem(picture_103007_png, sizeof(picture_103007_png));
        手持图片[103008] = createTexture_ALL_FromMem(picture_103008_png, sizeof(picture_103008_png));
        手持图片[103009] = createTexture_ALL_FromMem(picture_103009_png, sizeof(picture_103009_png));
        手持图片[103010] = createTexture_ALL_FromMem(picture_103010_png, sizeof(picture_103010_png));
        手持图片[103011] = createTexture_ALL_FromMem(picture_103011_png, sizeof(picture_103011_png));
        手持图片[103012] = createTexture_ALL_FromMem(picture_103012_png, sizeof(picture_103012_png));
        手持图片[103013] = createTexture_ALL_FromMem(picture_103013_png, sizeof(picture_103013_png));
        手持图片[103014] = createTexture_ALL_FromMem(picture_103014_png, sizeof(picture_103014_png));
        手持图片[103015] = createTexture_ALL_FromMem(picture_103015_png, sizeof(picture_103015_png));
        手持图片[103100] = createTexture_ALL_FromMem(picture_103100_png, sizeof(picture_103100_png));
        手持图片[103901] = createTexture_ALL_FromMem(picture_103901_png, sizeof(picture_103901_png));
        手持图片[103902] = createTexture_ALL_FromMem(picture_103902_png, sizeof(picture_103902_png));
        手持图片[103903] = createTexture_ALL_FromMem(picture_103903_png, sizeof(picture_103903_png));
        手持图片[104001] = createTexture_ALL_FromMem(picture_104001_png, sizeof(picture_104001_png));
        手持图片[104002] = createTexture_ALL_FromMem(picture_104002_png, sizeof(picture_104002_png));
        手持图片[104003] = createTexture_ALL_FromMem(picture_104003_png, sizeof(picture_104003_png));
        手持图片[104004] = createTexture_ALL_FromMem(picture_104004_png, sizeof(picture_104004_png));
        手持图片[104100] = createTexture_ALL_FromMem(picture_104100_png, sizeof(picture_104100_png));
        手持图片[105001] = createTexture_ALL_FromMem(picture_105001_png, sizeof(picture_105001_png));
        手持图片[105002] = createTexture_ALL_FromMem(picture_105002_png, sizeof(picture_105002_png));
        手持图片[105010] = createTexture_ALL_FromMem(picture_105010_png, sizeof(picture_105010_png));
        手持图片[106001] = createTexture_ALL_FromMem(picture_106001_png, sizeof(picture_106001_png));
        手持图片[106002] = createTexture_ALL_FromMem(picture_106002_png, sizeof(picture_106002_png));
        手持图片[106003] = createTexture_ALL_FromMem(picture_106003_png, sizeof(picture_106003_png));
        手持图片[106004] = createTexture_ALL_FromMem(picture_106004_png, sizeof(picture_106004_png));
        手持图片[106005] = createTexture_ALL_FromMem(picture_106005_png, sizeof(picture_106005_png));
        手持图片[106006] = createTexture_ALL_FromMem(picture_106006_png, sizeof(picture_106006_png));
        手持图片[106007] = createTexture_ALL_FromMem(picture_106007_png, sizeof(picture_106007_png));
        手持图片[106008] = createTexture_ALL_FromMem(picture_106008_png, sizeof(picture_106008_png));
        手持图片[106010] = createTexture_ALL_FromMem(picture_106010_png, sizeof(picture_106010_png));
        手持图片[107001] = createTexture_ALL_FromMem(picture_107001_png, sizeof(picture_107001_png));
        手持图片[107007] = createTexture_ALL_FromMem(picture_107007_png, sizeof(picture_107007_png));
        手持图片[108001] = createTexture_ALL_FromMem(picture_108001_png, sizeof(picture_108001_png));
        手持图片[108002] = createTexture_ALL_FromMem(picture_108002_png, sizeof(picture_108002_png));
        手持图片[108003] = createTexture_ALL_FromMem(picture_108003_png, sizeof(picture_108003_png));
        手持图片[108004] = createTexture_ALL_FromMem(picture_108004_png, sizeof(picture_108004_png));
        手持图片[602001] = createTexture_ALL_FromMem(picture_602001_png, sizeof(picture_602001_png));
        手持图片[602002] = createTexture_ALL_FromMem(picture_602002_png, sizeof(picture_602002_png));
        手持图片[602003] = createTexture_ALL_FromMem(picture_602003_png, sizeof(picture_602003_png));
        手持图片[602004] = createTexture_ALL_FromMem(picture_602004_png, sizeof(picture_602004_png));
        手持图片[105015] = createTexture_ALL_FromMem(picture_105015_png, sizeof(picture_105015_png));
        
        手持图片[602003] = createTexture_ALL_FromMem(rsp, sizeof(rsp));
        手持图片[602002] = createTexture_ALL_FromMem(ywd, sizeof(ywd));
        手持图片[602004] = createTexture_ALL_FromMem(手雷, sizeof(手雷));
        手持图片[602001] = createTexture_ALL_FromMem(zhen, sizeof(zhen));
        手持图片[602075] = createTexture_ALL_FromMem(铝热弹, sizeof(铝热弹));
}

#endif