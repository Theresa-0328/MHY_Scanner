#pragma once

#include <QImage>
#include <QtCore>
#include <opencv2/opencv.hpp>
#include <qrcodegen.hpp>

inline cv::Mat createQrCodeToCvMat(const std::string_view qrcodeString)
{
    const qrcodegen::QrCode qr{ qrcodegen::QrCode::encodeText(qrcodeString.data(), qrcodegen::QrCode::Ecc::QUARTILE) };
    const int size{ qr.getSize() };
    const int scale{ 5 };
    const int padding{ 0 };
    cv::Mat qrImage(size * scale + padding * 2, size * scale + padding * 2, CV_8UC1, cv::Scalar(255));
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            const bool isDark = qr.getModule(x, y);
            const cv::Rect roiRect((x * scale) + padding, (y * scale) + padding, scale, scale);
            cv::Mat roi = qrImage(roiRect);
            roi.setTo(isDark ? 0 : 255);
        }
    }
    return qrImage;
}

inline QImage CV_8UC1_MatToQImage(const cv::Mat& mat)
{
    QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);

    image.setColorCount(256);
    for (int i = 0; i < 256; i++)
    {
        image.setColor(i, qRgb(i, i, i));
    }

    uchar* pSrc = mat.data;
    for (int row = 0; row < mat.rows; row++)
    {
        uchar* pDest = image.scanLine(row);
        memcpy(pDest, pSrc, mat.cols);
        pSrc += mat.step;
    }

    return image;
}