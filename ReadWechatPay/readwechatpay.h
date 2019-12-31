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
	// Qualifier: ��������ͼƬ
	// Parameter: string num
	//************************************
	std::wstring MixNumber(std::string num, SIZE size);

	//************************************
	// Method:    CSSSprite
	// FullName:  ReadWechatPay::CSSSprite
	// Access:    public 
	// Returns:   void
	// Qualifier: �������ͼƬ
	// Parameter: wstring destpath ��ͼ
	// Parameter: wstring sourcePath ����ͼƬ
	// Parameter: const SIZE & size λ��
	//************************************
	std::wstring CSSSprite(std::wstring destpath, std::wstring sourcePath, const SIZE & size);

	//************************************
	// Method:    readImage
	// FullName:  ReadWechatPay::readImage
	// Access:    public 
	// Returns:   void
	// Qualifier: ���պϲ�
	// Parameter: wstring destpath ��ͼ
	// Parameter: wstring sourcePath ����ͼ
	//************************************
	void MixImage(std::wstring destpath, std::wstring sourcePath, const SIZE& size);

	//************************************
	// Method:    GetImageSize
	// FullName:  ReadWechatPay::GetImageSize
	// Access:    public 
	// Returns:   SIZE
	// Qualifier: ��ȡͼƬ�ߴ�
	// Parameter: std::wstring path
	//************************************
	SIZE GetImageSize(std::wstring path);

	//************************************
	// Method:    TransfomeImage
	// FullName:  ReadWechatPay::TransfomeImage
	// Access:    public 
	// Returns:   int
	// Qualifier: ��ͼƬת��ΪBmp��ʽ  //��ת������ͼƬ��ʽ
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

	std::string _num_str; //��ֵ
	SIZE devOrigin;

	int nameSerial = 0;
};

#endif // READWECHATPAY_H
