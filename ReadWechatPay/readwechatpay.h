#pragma once

#ifndef READWECHATPAY_H
#define READWECHATPAY_H

#include <Windows.h>
#include <QtWidgets/QMainWindow>
#include "GeneratedFiles/ui_readwechatpay.h"

class ReadWechatPay : public QMainWindow
{
	Q_OBJECT

public:
	ReadWechatPay(QWidget *parent = 0);
	~ReadWechatPay();

	int InIt();

	//************************************
	// Method:    MixNumber
	// FullName:  ReadWechatPay::MixNumber
	// Access:    public 
	// Returns:   void
	// Qualifier: 生成数字图片
	// Parameter: string num
	//************************************
	std::wstring MixNumber(std::string num, SIZE size);

	//************************************
	// Method:    CSSSprite
	// FullName:  ReadWechatPay::CSSSprite
	// Access:    public 
	// Returns:   void
	// Qualifier: 混合两张图片
	// Parameter: wstring destpath 底图
	// Parameter: wstring sourcePath 插入图片
	// Parameter: const SIZE & size 位置
	//************************************
	std::wstring CSSSprite(std::wstring destpath, std::wstring sourcePath, const SIZE & size);

	//************************************
	// Method:    readImage
	// FullName:  ReadWechatPay::readImage
	// Access:    public 
	// Returns:   void
	// Qualifier: 最终合并
	// Parameter: wstring destpath 底图
	// Parameter: wstring sourcePath 数字图
	//************************************
	void MixImage(std::wstring destpath, std::wstring sourcePath, const SIZE& size);

	//************************************
	// Method:    GetImageSize
	// FullName:  ReadWechatPay::GetImageSize
	// Access:    public 
	// Returns:   SIZE
	// Qualifier: 获取图片尺寸
	// Parameter: std::wstring path
	//************************************
	SIZE GetImageSize(std::wstring path);

	//************************************
	// Method:    TransfomeImage
	// FullName:  ReadWechatPay::TransfomeImage
	// Access:    public 
	// Returns:   int
	// Qualifier: 将图片转换为Bmp格式  //可转换任意图片格式
	// Parameter: std::wstring path
	//************************************
	int TransfomeImage(std::wstring path);

	int GetEncoderClsid(const WCHAR * format, CLSID * pClsid);

	std::wstring recursionMix(std::wstring path, const SIZE size);


public slots:
	void on_selectButton_clicked();
	void on_pushButton_clicked();

private:
	Ui::ReadWechatPayClass ui;

	std::string _num_str; //数值
	SIZE devOrigin;

	int nameSerial = 0;
};

#endif // READWECHATPAY_H
