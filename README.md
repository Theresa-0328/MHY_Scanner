# MHY_Scanner

<div align="center">

[![GitHub stars](https://img.shields.io/github/stars/Theresa-0328/MHY_Scanner?color=blue&style=for-the-badge)](https://github.com/Theresa-0328/MHY_Scanner/stargazers)
</div>

### **版本 - v1.1.1**

### [国内蓝奏云下载链接](https://wwru.lanzouc.com/b04e121qj) 密码:afph

## 功能和特性
- 崩坏3，原神，星穹铁道通用的Windows平台的扫码和抢码登录器。
- GUI界面。
- 从屏幕自动获取二维码登录，适用于大部分登录情景，不适用于在竞争激烈时抢码。
- 从直播流获取二维码登录，适用于抢码登录情景。
- 可选启动后自动开始识别屏幕和识别完成后自动退出，无需登录后手动切窗口关闭。
- 表格化管理多账号，方便切换游戏账号。
- 对于崩坏3 bilibili服，本项目使用你原本的用户名登录并显示在游戏客户端，不强制修改为用户名为"崩坏3扫码器"。
## 目前可用的服务器
| 崩坏3 | 原神 | 星穹铁道 |
|:-----:|:-----:|:-----:|
| 官服| 官服 | 官服 |
| bilibili服 |  |  |

## 使用说明
下载解压后运行 MHY_Scanner.exe

添加账号，官服使用cookie登录，崩坏3B服账号密码。本地仅保存登录信息在config/uesrinfo.json，不保存cookie或密码。

如果你不知道怎么获得米游社Cookie，[点击这里](./doc/Cookie.md)

登陆后点击 **开始监视屏幕** 就可以自动识别任意显示在屏幕上的二维码并自动登录，再次点击可以主动停止，在发生登录后也会停止。

在当前直播间输入框输入需要抢二维码的直播间号，点击 **开始监视直播间** 就可以自动识别该直播间显示的二维码并自动登录，再次点击可以主动停止，在发生登录后也会停止。

直播间号仅支持BiliBili直播间，是一串纯数字，比如你想抢的直播间的链接是 https://live.bilibili.com/6, 那么直播间号为6。

不支持同时从屏幕自动获取二维码登录和从直播获取二维码登录，启动其中一种方法会自动停止另一方法。

目前没有进行大量测试，如果有任何建议和发现的问题欢迎提Issues。

## 常见问题
1. 崩坏3从BiliBili服切换到官服时会游戏客户端可能会提示"渠道错误"。

&emsp;该错误与本项目无关。
## 依赖
- qt v6.2.3
- curl v7.87.0
- opencv v4.80
- openssl v3.10
- ffmpeg v6.0

## 未来目标
- [ ] 增加日志系统
- [ ] 优化UI
- [ ] 支持更多平台
- [ ] 优化cpu占用

## 参考和感谢
- [BililiveRecorder/BililiveRecorder](https://github.com/BililiveRecorder/BililiveRecorder)

- [HonkaiScanner/scannerHelper](https://github.com/HonkaiScanner/scannerHelper)
