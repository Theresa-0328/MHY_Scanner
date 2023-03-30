#include"main.h"
#include"Bsgsdk.h"
using namespace json;
using namespace std;
int main()
{
	utils u;
	Bsgsdk b;
	Json j;
	string res;
	map<string, string>m;
	m.insert({ "pwd","参数" });
	//res = "hello你好";
	//int i;
	//u.PostRequest("http://httpbin.org/post", "", res);
	//i = u.getCurrentUnixTime();
	j = b.getUserInfo("1","2");
	//res = "hello";
	//res = b.setSign(m);
	//res = u.urlEncode(res);
	//j.parse("{ \"pwd\":\"参数\" }");
	//res = b.setSign(j);
	cout << res << endl;

}