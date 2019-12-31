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
	//读取图片信息
	//获取数字所在位置（调用百度文字识别api）
	//根据数字生成插入图片
	return 0;
}
void ReadWechatPay::on_pushButton_clicked() {

	QString imagePath = ui.lineEdit->text();

	AppIni *ini = AppIni::GetInstance();
	wstring destpath = imagePath.toStdWString();  //需要处理的图片
	wstring sourcePath = ini->GetAppPath() + L"\\num.png"; //TODO 生成的数字
	if (destpath.empty()) {
		MessageBox(nullptr, L"先选择图片", L"提示", NULL);
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

	wstring shadePath = AppIni::GetInstance()->GetAppPath() + L"\\wechatpaynum\\shade.png";//底纹路径
	wstring minusPath = AppIni::GetInstance()->GetAppPath() + L"\\wechatpaynum\\minus.png";//减号

	wstring ret = CSSSprite(shadePath, minusPath, size);
	if (ret.empty()) {
		MessageBox(nullptr, L"首次混合失败", L"提醒", NULL);
		return L"";
	}
	size.cx += MINUSWIDTH + MINUSSPACE;
	devOrigin = size;
	//完成混合
	return recursionMix(ret, devOrigin);  //获取最终数字图片路径

}

wstring ReadWechatPay::CSSSprite(wstring destpath, wstring sourcePath, const SIZE& size) {
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	// GDI+的任何操作之前应该先初始化
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Image *IimageFile = new Image(destpath.c_str());
	Image *IsourcePath = new Image(sourcePath.c_str());
	IsourcePath->GetWidth();

	//操作前需要初始化GDI+
	//Gdiplus::Bitmap *dest = new Gdiplus::Bitmap(L"C:\\Users\\Administrator\\Desktop\\Bird.bmp");//TODO 无法识别路径
	//Gdiplus::Bitmap *source = new Gdiplus::Bitmap(sourcePath.c_str());

	if (IimageFile && IsourcePath) {

		Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(IimageFile);
		if (g) {
			// 使用高质量模式(相对比较耗时)，可以查看msdn，替换为其他mode   
			g->SetInterpolationMode(InterpolationModeHighQualityBicubic); //
			g->DrawImage(IsourcePath, size.cx, size.cy, IsourcePath->GetWidth(), IsourcePath->GetHeight());
			delete g;
			g = nullptr;
		}
		else {
			MessageBox(NULL, L"画笔初始化失败", L"提示", NULL);
			return L"";
		}
	}
	else
	{
		MessageBox(NULL, L"读取图片失败", L"提示", NULL);
		return L"";
	}

	CLSID   encoderClsid;
	Status  stat;
	// 获取要转换格式的CLSID
	GetEncoderClsid(L"image/bmp", &encoderClsid);
	// 根据CLSID保存为对应格式的图片
	vector<string> splitPath = Util::split(Util::ws2s(destpath), ".");
	
	wstring exportPath = AppIni::GetInstance()->GetAppPath() + L"\\cache\\" + Util::int2wstr(nameSerial++) + L".png";
	stat = IimageFile->Save(exportPath.c_str(), &encoderClsid, NULL);

	if (stat != Ok)
	{
		MessageBox(NULL, L"合并失败", L"提示", NULL);
		return L"";
	}

	delete IimageFile;
	delete IsourcePath;
	// 关闭GDI+
	Gdiplus::GdiplusShutdown(gdiplusToken);
	//DeleteFile(destpath.c_str());
	return exportPath;
}

void ReadWechatPay::MixImage(wstring destpath, wstring sourcePath, const SIZE& size) {

	//TODO
	//根据输入数字混合图片
	QString num = ui.lineEdit_2->text();
	if (num.isEmpty()) {
		MessageBox(NULL, L"先输入要修改的数字，包含小数点", L"提示", NULL);
		return;
	}
	_num_str = num.toStdString();
	//默认是减（支出）
	vector<string> v_num = Util::split(num.toStdString(), ".");
	//如果小数点后没有数据可能会崩
	if (v_num.size() <= 1 || v_num.empty()) {
		MessageBox(NULL, L"数据格式有误，需要包含小数点及小数点后数据", L"提示", NULL);
		return;
	}
	//计算数据共需要多少像素 长度
	int dotfront = (v_num[0].length() - 1);
	int dotback = (v_num[1].length() - 1);
	int sumlength = MINUSWIDTH + MINUSSPACE + dotfront * NUMSPACE + (dotfront + 1)*NUMWIDTH + DOTSPACE*2 + DOTWIDTH + dotback*NUMSPACE + (dotback + 1)*NUMWIDTH;
	int i_old_origin = (int)SHADEWIDTH / 2 - sumlength / 2;		
	SIZE origin = { i_old_origin, 0 };			//图片插入起点
	wstring retNumber = MixNumber(num.toStdString(), origin);  //获取数据图片
	
	if (!retNumber.empty()) {
		//计算偏移量
		int i_user_height = Util::ws2int(ui.userHeight->text().toStdWString());//用户输入高度
		int i_user_offset = Util::ws2int(ui.offsetEdit->text().toStdWString());//用户输入偏移量

		//获取原始图片宽高
		SIZE retSize = GetImageSize(destpath);
		SIZE coor = { (int)retSize.cx/2 - (int)SHADEWIDTH/2 - i_user_offset, i_user_height };//高度需要请求百度api字体识别或手动输入
		wstring exportPath = CSSSprite(destpath, retNumber, coor);
		if (!exportPath.empty()) {
			//导出成功 查看图片
			auto userClick = MessageBox(NULL, L"合并成功，生成图片在cache目录下，是否打开文件夹", L"成功", MB_OKCANCEL);
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
	// GDI+的任何操作之前应该先初始化
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
通过字符串获得对应的CLSID
format:图像格式,如：image/bmp
pClsid:存放对应的CLSID
成功返回对应的编码器位置,失败返回-1.
*/
int ReadWechatPay::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // 图像编码器数量
	UINT  size = 0;         // 图像编码器数组大小

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);    // 获取编码器数量
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);    // 获取本机支持的编码器

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)    // 找到该格式就将对应的CLSID给*pClsid
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

