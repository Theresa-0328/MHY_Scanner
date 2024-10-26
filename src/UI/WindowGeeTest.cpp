#include "WindowGeeTest.h"

#include <iostream>
#include <format>

WindowGeeTest::WindowGeeTest(QWidget* parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::Window);
    setMinimumSize(QSize(400, 450));
    setWindowTitle("请完成验证");
}

WindowGeeTest::~WindowGeeTest()
{
    if (m_webViewController)
    {
        m_webViewController->Close();
    }
}

void WindowGeeTest::closeEvent(QCloseEvent* event)
{
    if (m_webViewController)
    {
        m_webViewController->Close();
    }
}

void WindowGeeTest::Init(const std::wstring_view gt, const std::wstring_view challenge)
{
    constexpr std::wstring_view TemplateHtml{ LR"(
            <html>
                <head>
                    <title>GeeTest</title>
                    <style>
                        #geetest-div {{
                            aligin-items:center
                        }}
                    </style>
                </head>
                <body>
                    <div id="geetest-div"></div>
                </body>
                <script src="https://static.geetest.com/static/js/gt.0.5.2.js"></script>
                <script>
                    initGeetest(
                        {{
                            protocol: "https://",
                            gt: "{}",
                            challenge: "{}",
                            new_captcha: true,
                            product: 'bind',
                            api_server: 'api.geetest.com'
                        }},
                        function (captchaObj) {{
                            captchaObj.onReady(function () {{
                                captchaObj.verify();
                            }});
                            captchaObj.onSuccess(function () {{
                                var result = captchaObj.getValidate();
                                chrome.webview.postMessage(result);
                            }});
                        }}
                    );
              </script>
            </html>)" };
    indexHtml = std::format(TemplateHtml, gt, challenge);
}

void WindowGeeTest::showEvent(QShowEvent* event)
{
    CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>([this](HRESULT error, ICoreWebView2Environment* env) {
            if (!!error)
            {
                return error;
            }

            env->CreateCoreWebView2Controller(reinterpret_cast<HWND>(winId()),
                                              Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>([this](HRESULT error, ICoreWebView2Controller* controller) {
                                                  if (!!error)
                                                  {
                                                      return error;
                                                  }
                                                  m_webViewController = controller;
                                                  m_webViewController->get_CoreWebView2(&m_webView);
                                                  m_webViewController->put_Bounds({ 0, 0, width(), height() });

                                                  m_webView->get_Settings(&m_settings);
                                                  m_settings->put_IsStatusBarEnabled(false);

                                                  m_webView->NavigateToString(indexHtml.c_str());
                                                  //m_webView->Navigate(L"https://www.bing.com/");

                                                  m_webView->add_NewWindowRequested(
                                                      Microsoft::WRL::Callback<ICoreWebView2NewWindowRequestedEventHandler>(
                                                          [this](ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args) {
                                                              args->put_Handled(TRUE);
                                                              return S_OK;
                                                          })
                                                          .Get(),
                                                      &m_webResourceRequestedToken);

                                                  m_webView->add_WebMessageReceived(
                                                      Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                                                          [this](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) {
                                                              LPWSTR message;
                                                              args->get_WebMessageAsJson(&message);
                                                              emit postMessage(QString::fromWCharArray(message));
                                                              CoTaskMemFree(message);
                                                              return S_OK;
                                                          })
                                                          .Get(),
                                                      &m_webResourceRequestedToken);
                                              }).Get());
        }).Get());
}
