# MHY_Scanner

<div align="center">

[![GitHub stars](https://img.shields.io/github/stars/Theresa-0328/MHY_Scanner?color=blue&style=for-the-badge)](https://github.com/Theresa-0328/MHY_Scanner/stargazers)
</div>

### **版本 - v1.1.11**

## 重要提醒
最近在闲鱼等地发现有人在卖这个项目，希望没有人真的傻逼到花钱买免费的代码。如果你是购买来的，说明你被骗了，建议联系商家退款。最后，如果你真的想卖本项目赚钱，<a href="https://www.baidu.com/s?wd=%E5%AD%A4%E5%84%BF%E6%80%8E%E4%B9%88%E5%8A%9E%E6%88%B7%E5%8F%A3%E6%9C%AC">请点这里了解</a> 

## 功能和特性
- 从屏幕自动获取二维码登录，适用于大部分登录情景，不适用于在竞争激烈时抢码。
- 从直播流获取二维码登录，适用于抢码登录情景。
- 可选启动后自动开始识别屏幕和识别完成后自动退出，无需登录后手动切窗口关闭。
- 表格化管理多账号，方便切换游戏账号。

## 目前可用的平台
|   崩坏3    | 原神  | 星穹铁道 |
| :--------: | :---: | :------: |
|    官服    | 官服  |   官服   |
| bilibili服 |       |          |

## 使用说明
下载解压后运行 MHY_Scanner.exe

点击菜单栏 **账号管理->添加账号**，添加你的账号。

双击账号对应的备注单元格可以添加自定义备注。

登陆后点击 **监视屏幕** 就可以自动识别任意显示在屏幕上的二维码并自动登录。

选择你需要的直播平台,在当前直播间输入框输入`RID`，点击 **监视直播间** 就可以自动识别该直播间显示的二维码并自动登录。

正在执行的任务的按钮会高亮显示，再次点击会停止。

`RID`是纯数字，一般从直播间链接中获得。

|                平台                |           `<RID>` 位置            |
| :--------------------------------: | :-------------------------------: |
| [B 站](https://live.bilibili.com/) | `https://live.bilibili.com/<RID>` |
|  [抖音](https://live.douyin.com/)  |  `https://live.douyin.com/<RID>`  |
|     [虎牙](https://huya.com/)      |   `https://www.huya.com/<RID>`    |

目前没有进行大量测试，如果有任何建议和问题欢迎提Issues。

## 常见问题
- 启动报错无法定位程序输入点。说明你没有安装最新的vc运行时库。[下载最新支持的Visual C++](https://aka.ms/vs/17/release/vc_redist.x64.exe)，详细解释查看[Microsoft官方文档](https://learn.microsoft.com/zh-cn/cpp/windows/latest-supported-vc-redist?view=msvc-170)。

## 依赖
- qt v6.2.3
- curl v8.2.1
- opencv v4.80
- openssl v3.10
- ffmpeg v6.0
- googletest
- boost
- httplib

## 参考和感谢
- [BililiveRecorder/BililiveRecorder](https://github.com/BililiveRecorder/BililiveRecorder)

- [HonkaiScanner](https://github.com/HonkaiScanner)
