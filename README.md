# Honkai3StreamQRCode

<div align="center">

[![GitHub stars](https://img.shields.io/github/stars/Theresa-0328/Honkai3StreamQRCode?color=blue&style=for-the-badge)](https://github.com/Theresa-0328/Honkai3StreamQRCode/stargazers)
</div>

### **版本 - V0.0.1-bate**

### [国内蓝奏云下载链接](https://wwru.lanzouf.com/i2gwj0uyok0j)

## 功能和特性
- 用于崩坏3扫码和抢码登录。
- GUI界面
- 从屏幕自动获取二维码登录，适用于大部分登录情景，不适用于在竞争激烈时抢码。
- 从直播流获取二维码登录，适用于抢码登录情景。
- 可选启动后自动开始识别屏幕和识别完成后自动退出
- 
## 支持的崩坏3服务器
- b服

## 简易使用指导
运行 BH3ScannerGui.exe

首次使用登录后，用户信息会保存在config/config_private.json
,后续会在启动时自动登录。

登陆后点击 **开始监视屏幕** 就可以自动识别任意显示在屏幕上的崩坏3二维码，再次点击可以主动停止，在发生登录后也会停止。

在当前直播间输入框输入需要抢二维码的直播间号，点击 **开始监视直播间** 就可以获取指定直播间显示的崩坏3二维码，再次点击可以主动停止，在发生登录后也会停止。

直播间号是一串纯数字，比如你想抢的直播间的链接是 https://live.bilibili.com/6, 那么直播间号为6。

目前不支持同时从屏幕自动获取二维码登录和从直播获取二维码登录，启动其中一种方法会自动停止另一方法。

Config文件夹里的config_private.json是用户信息文件，没有充分理解不要随意手动更改，随意更改会导致无法启动。

cache文件夹里的output.flv是缓存的直播视频，程序退出后可以手动删除。

如果启动失败建议检查网络链接是否正常，Config文件夹里是否存在config_private.json

目前没有进行大量测试，如果有任何建议和发现的问题欢迎提Issues或发送邮件到CrystalWhisperer@outlook.com

## 依赖
- qtmsvc v6.2.3
- curl v7.87.0
- opencv v4.70
- openssl v3.10
- ffmpeg

## 计划
- 支持更多崩坏3服务器
- 优化cpu占用

## 参考和感谢
- [BililiveRecorder/BililiveRecorder](https://github.com/BililiveRecorder/BililiveRecorder)

- [HonkaiScanner/scannerHelper](https://github.com/HonkaiScanner/scannerHelper)
