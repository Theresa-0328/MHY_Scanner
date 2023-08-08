#include <QMessageBox>
#include "LoginBili.h"
#include "ThreadLocalServer.h"
LoginBili::LoginBili()
{

}

LoginBili::~LoginBili()
{

}

//获取用户名,检查access_key是否有效
int LoginBili::loginBiliKey(std::string& name, const std::string& uid, const std::string& access_key)
{
	json::Json userInfo = getUserInfo(uid, access_key);
	int code = (int)userInfo["code"];
	const std::string uname = userInfo["uname"];
	if (code != 0)
	{
		return code;
	}
	name = uname;
	return 0;
}

int LoginBili::loginBiliPwd(std::string Account, std::string Pwd,
	std::string& message, std::string& uid, std::string& access_key, std::string& name)
{
	json::Json loginJ;
	std::string loginInfo = login(Account, Pwd);
	loginJ.parse(loginInfo);
	int code = (int)loginJ["code"];
	if (code == 200000)
	{
		ThreadLocalServer thserver;
		thserver.start();
		const std::string url = makeCaptchUrl();
		const QString Qurl = QString::fromStdString(url);
		ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
		QMessageBox msgBox;
		msgBox.setWindowTitle("验证提示");
		msgBox.setTextInteractionFlags(Qt::TextSelectableByMouse);
		msgBox.setText("如果没有自动打开浏览器，请手动复制并打开后面的链接并进行验证" + Qurl + "如果你完成了验证码，点击OK");
		msgBox.exec();
		thserver.stop();
		if (thserver.reCaptcha() == "")
		{
			message = loginJ["message"];
			return code;
		}
		json::Json captcha;
		captcha.parse(thserver.reCaptcha());
		std::string challenge = captcha["challenge"];
		std::string gt_user = captcha["userid"];
		std::string validate = captcha["validate"];
		loginInfo = login(Account, Pwd, challenge, gt_user, validate);
		loginJ.parse(loginInfo);
		code = (int)loginJ["code"];
		std::cout << loginJ.str() << std::endl;
	}
	if (code != 0)
	{
		message = loginJ["message"];
		return code;
	}
	uid = std::to_string((int)loginJ["uid"]);
	access_key = loginJ["access_key"];
	loginJ.clear();
	json::Json userInfo = getUserInfo(uid, access_key);
	name = userInfo["uname"];
	return code;
}