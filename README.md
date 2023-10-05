# MHY_Scanner

<div align="center">

[![GitHub stars](https://img.shields.io/github/stars/Theresa-0328/MHY_Scanner?color=blue&style=for-the-badge)](https://github.com/Theresa-0328/MHY_Scanner/stargazers)
</div>

### **版本 - v1.1.3**

## 重要提醒
最近在闲鱼等地发现有人在卖这个项目，希望没有人真的傻逼到花钱买免费的代码。如果你是购买来的，说明你被骗了，建议联系商家退款。最后，如果你真的想卖本项目赚钱，<a href="https://www.baidu.com/s?wd=%E5%AD%A4%E5%84%BF%E6%80%8E%E4%B9%88%E5%8A%9E%E6%88%B7%E5%8F%A3%E6%9C%AC">请点这里了解</a> 

## 功能和特性
- 崩坏3，原神，星穹铁道通用的Windows平台的扫码和抢码登录器。
- GUI界面。
- 从屏幕自动获取二维码登录，适用于大部分登录情景，不适用于在竞争激烈时抢码。
- 从直播流获取二维码登录，适用于抢码登录情景。
- 可选启动后自动开始识别屏幕和识别完成后自动退出，无需登录后手动切窗口关闭。
- 表格化管理多账号，方便切换游戏账号。
- 对于崩坏3 bilibili服，本项目使用你原本的用户名登录并显示在游戏客户端，不强制修改为用户名为"崩坏3扫码器"。
## 目前可用的平台
| 崩坏3 | 原神 | 星穹铁道 |
|:-----:|:-----:|:-----:|
| 官服| 官服 | 官服 |
| bilibili服 |  |  |

## 使用说明
下载解压后运行 MHY_Scanner.exe

添加账号，官服使用cookie登录，崩坏3B服账号密码。本地仅保存登录信息在config/uesrinfo.json，不保存cookie或密码。

如果你不知道怎么获得米游社Cookie，[点击这里](./doc/Cookie.md)

登陆后点击 **开始监视屏幕** 就可以自动识别任意显示在屏幕上的二维码并自动登录，再次点击可以主动停止，在发生登录后也会停止。

选择你需要的直播平台,在当前直播间输入框输入`RID`，点击 **开始监视直播间** 就可以自动识别该直播间显示的二维码并自动登录，再次点击可以主动停止，在发生登录后也会停止。

`RID`是纯数字，一般从直播间链接中获得。

| 平台| `<RID>` 位置 |
|:--------------------:|:--------------------------------------:|
|[B 站](https://live.bilibili.com/)|  `https://live.bilibili.com/<RID>`     |
|[抖音](https://live.douyin.com/)    |   `https://live.douyin.com/<RID>`     |
|[虎牙](https://huya.com/) |`https://www.huya.com/<RID>`  |

不支持同时从屏幕自动获取二维码登录和从直播获取二维码登录，启动其中一种方法会自动停止另一方法。

目前没有进行大量测试，如果有任何建议和问题欢迎提Issues。

## 常见问题
1. 崩坏3从BiliBili服切换到官服时会游戏客户端可能会提示"渠道错误"。

&emsp;该错误与本项目无关。
## 依赖
- qt v6.2.3
- curl v8.2.1
- opencv v4.80
- openssl v3.10
- ffmpeg v6.0
- googletest
- boost
- httplib

## 未来目标
- [ ] 增加日志系统
- [ ] 优化UI
- [ ] 支持更多平台
- [ ] 优化cpu占用

## 参考和感谢
- [BililiveRecorder/BililiveRecorder](https://github.com/BililiveRecorder/BililiveRecorder)

- [HonkaiScanner/scannerHelper](https://github.com/HonkaiScanner/scannerHelper)
