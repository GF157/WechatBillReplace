#pragma once
#include "readwechatpay.h"
#include "stdafx.h"
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")

using namespace std;
using namespace Gdiplus;

ReadWechatPay::ReadWechatPay(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.userHeight->setText("438");
	ui.offsetEdit->setText("0");
}

ReadWechatPay::~ReadWechatPay()
{

}

int ReadWechatPay::InIt() {
	show();
	//��ȡͼƬ��Ϣ
	//��ȡ��������λ�ã����ðٶ�����ʶ��api��
	//�����������ɲ���ͼƬ
	return 0;
}
void ReadWechatPay::on_pushButton_clicked() {

	QString imagePath = ui.lineEdit->text();

	AppIni *ini = AppIni::GetInstance();
	wstring destpath = imagePath.toStdWString();  //��Ҫ�����ͼƬ
	wstring sourcePath = ini->GetAppPath() + L"\\num.png"; //TODO ���ɵ�����
	if (destpath.empty()) {
		MessageBox(nullptr, L"��ѡ��ͼƬ", L"��ʾ", NULL);
		return;
	}
	SIZE sizeOrigin = { 0, 0 };
	MixImage(destpath, sourcePath, sizeOrigin);
}

void ReadWechatPay::on_selectButton_clicked()
{
	OPENFILENAME ofn;
	char szFile[300];
	std::wstring wstrVideo;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);

	ofn.lpstrFilter = L"Image(*.bmp;*.jpg;*.png;*.gif;*.jpeg;*.tga)\0*.bmp;*.jpg;*.png;*.gif;*.jpeg;*.tga\0";

	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		std::wstring selectPath = ofn.lpstrFile;

		ui.lineEdit->setText(QString::fromLocal8Bit(Util::ws2s(selectPath).c_str()));
	}
}

wstring ReadWechatPay::recursionMix(wstring path, const SIZE size) {

	wstring insertPath;
	wstring retPath = path;
	for (auto s : _num_str) {
		string str(1, s);
		if (strcmp(str.c_str(), ".") == 0) {			
			insertPath = AppIni::GetInstance()->GetAppPath() + L"\\wechatpaynum\\dot.png";
			
			retPath = CSSSprite(retPath, insertPath, devOrigin);
			devOrigin.cx += DOTSPACE + DOTWIDTH;

		}
		else
		{
			insertPath = AppIni::GetInstance()->GetAppPath() + L"\\wechatpaynum\\" + Util::s2ws(str) + L".png";
			
			retPath = CSSSprite(retPath, insertPath, devOrigin);
			devOrigin.cx += NUMWIDTH + NUMSPACE;
			if (Util::str2int(str) == 1)
			{
				devOrigin.cx -= 2;
			}
			else if (Util::str2int(str) == 4)
			{
				devOrigin.cx += 2;
			}

		}
		if (retPath.empty()) {
			MessageBox(nullptr, L"error", L"error", NULL);
			return L"";
		}
	}
	return retPath;
}

wstring ReadWechatPay::MixNumber(string num, SIZE size) {
	if (num.empty()) {
		return L"";
	}

	wstring shadePath = AppIni::GetInstance()->GetAppPath() + L"\\wechatpaynum\\shade.png";//����·��
	wstring minusPath = AppIni::GetInstance()->GetAppPath() + L"\\wechatpaynum\\minus.png";//����

	wstring ret = CSSSprite(shadePath, minusPath, size);
	if (ret.empty()) {
		MessageBox(nullptr, L"�״λ��ʧ��", L"����", NULL);
		return L"";
	}
	size.cx += MINUSWIDTH + MINUSSPACE;
	devOrigin = size;
	//��ɻ��
	return recursionMix(ret, devOrigin);  //��ȡ��������ͼƬ·��

}

wstring ReadWechatPay::CSSSprite(wstring destpath, wstring sourcePath, const SIZE& size) {
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	// GDI+���κβ���֮ǰӦ���ȳ�ʼ��
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Image *IimageFile = new Image(destpath.c_str());
	Image *IsourcePath = new Image(sourcePath.c_str());
	IsourcePath->GetWidth();

	//����ǰ��Ҫ��ʼ��GDI+
	//Gdiplus::Bitmap *dest = new Gdiplus::Bitmap(L"C:\\Users\\Administrator\\Desktop\\Bird.bmp");//TODO �޷�ʶ��·��
	//Gdiplus::Bitmap *source = new Gdiplus::Bitmap(sourcePath.c_str());

	if (IimageFile && IsourcePath) {

		Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(IimageFile);
		if (g) {
			// ʹ�ø�����ģʽ(��ԱȽϺ�ʱ)�����Բ鿴msdn���滻Ϊ����mode   
			g->SetInterpolationMode(InterpolationModeHighQualityBicubic); //
			g->DrawImage(IsourcePath, size.cx, size.cy, IsourcePath->GetWidth(), IsourcePath->GetHeight());
			delete g;
			g = nullptr;
		}
		else {
			MessageBox(NULL, L"���ʳ�ʼ��ʧ��", L"��ʾ", NULL);
			return L"";
		}
	}
	else
	{
		MessageBox(NULL, L"��ȡͼƬʧ��", L"��ʾ", NULL);
		return L"";
	}

	CLSID   encoderClsid;
	Status  stat;
	// ��ȡҪת����ʽ��CLSID
	GetEncoderClsid(L"image/bmp", &encoderClsid);
	// ����CLSID����Ϊ��Ӧ��ʽ��ͼƬ
	vector<string> splitPath = Util::split(Util::ws2s(destpath), ".");
	
	wstring exportPath = AppIni::GetInstance()->GetAppPath() + L"\\cache\\" + Util::int2wstr(nameSerial++) + L".png";
	stat = IimageFile->Save(exportPath.c_str(), &encoderClsid, NULL);

	if (stat != Ok)
	{
		MessageBox(NULL, L"�ϲ�ʧ��", L"��ʾ", NULL);
		return L"";
	}

	delete IimageFile;
	delete IsourcePath;
	// �ر�GDI+
	Gdiplus::GdiplusShutdown(gdiplusToken);
	//DeleteFile(destpath.c_str());
	return exportPath;
}

void ReadWechatPay::MixImage(wstring destpath, wstring sourcePath, const SIZE& size) {

	//TODO
	//�����������ֻ��ͼƬ
	QString num = ui.lineEdit_2->text();
	if (num.isEmpty()) {
		MessageBox(NULL, L"������Ҫ�޸ĵ����֣�����С����", L"��ʾ", NULL);
		return;
	}
	_num_str = num.toStdString();
	//Ĭ���Ǽ���֧����
	vector<string> v_num = Util::split(num.toStdString(), ".");
	//���С�����û�����ݿ��ܻ��
	if (v_num.size() <= 1 || v_num.empty()) {
		MessageBox(NULL, L"���ݸ�ʽ������Ҫ����С���㼰С���������", L"��ʾ", NULL);
		return;
	}
	//�������ݹ���Ҫ�������� ����
	int dotfront = (v_num[0].length() - 1);
	int dotback = (v_num[1].length() - 1);
	int sumlength = MINUSWIDTH + MINUSSPACE + dotfront * NUMSPACE + (dotfront + 1)*NUMWIDTH + DOTSPACE*2 + DOTWIDTH + dotback*NUMSPACE + (dotback + 1)*NUMWIDTH;
	int i_old_origin = (int)SHADEWIDTH / 2 - sumlength / 2;		
	SIZE origin = { i_old_origin, 0 };			//ͼƬ�������
	wstring retNumber = MixNumber(num.toStdString(), origin);  //��ȡ����ͼƬ
	
	if (!retNumber.empty()) {
		//����ƫ����
		int i_user_height = Util::ws2int(ui.userHeight->text().toStdWString());//�û�����߶�
		int i_user_offset = Util::ws2int(ui.offsetEdit->text().toStdWString());//�û�����ƫ����

		//��ȡԭʼͼƬ���
		SIZE retSize = GetImageSize(destpath);
		SIZE coor = { (int)retSize.cx/2 - (int)SHADEWIDTH/2 - i_user_offset, i_user_height };//�߶���Ҫ����ٶ�api����ʶ����ֶ�����
		wstring exportPath = CSSSprite(destpath, retNumber, coor);
		if (!exportPath.empty()) {
			//�����ɹ� �鿴ͼƬ
			auto userClick = MessageBox(NULL, L"�ϲ��ɹ�������ͼƬ��cacheĿ¼�£��Ƿ���ļ���", L"�ɹ�", MB_OKCANCEL);
			if (userClick == IDOK) {
				ShellExecute(NULL, L"explore", (AppIni::GetInstance()->GetAppPath() + L"\\cache").c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
			else
			{

			}
		}
	}
}


SIZE ReadWechatPay::GetImageSize(wstring path) {
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	// GDI+���κβ���֮ǰӦ���ȳ�ʼ��
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	Image *IimageFile = new Image(path.c_str());
	if (IimageFile) {
		SIZE size;
		size.cx = IimageFile->GetWidth();
		size.cy = IimageFile->GetHeight();
		return size;
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
}



/*
ͨ���ַ�����ö�Ӧ��CLSID
format:ͼ���ʽ,�磺image/bmp
pClsid:��Ŷ�Ӧ��CLSID
�ɹ����ض�Ӧ�ı�����λ��,ʧ�ܷ���-1.
*/
int ReadWechatPay::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // ͼ�����������
	UINT  size = 0;         // ͼ������������С

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);    // ��ȡ����������
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);    // ��ȡ����֧�ֵı�����

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)    // �ҵ��ø�ʽ�ͽ���Ӧ��CLSID��*pClsid
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

