#pragma once

#include <Windows.h>

#include <QWidget>
#include <WebView2.h>
#include <wil/com.h>
#include <wrl.h>

class WindowGeeTest : public QWidget
{
    Q_OBJECT
public:
    WindowGeeTest(QWidget* parent = nullptr);
    ~WindowGeeTest();
    void Init(const std::wstring_view gt, const std::wstring_view challenge);

signals:
    void postMessage(const QString& Message);

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    wil::com_ptr<ICoreWebView2Controller> m_webViewController{};
    wil::com_ptr<ICoreWebView2> m_webView{};
    EventRegistrationToken m_webResourceRequestedToken{};
    std::wstring indexHtml{};
};