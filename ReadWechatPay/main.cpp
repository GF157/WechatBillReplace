#include "readwechatpay.h"
#include <QtWidgets/QApplication>
#include "stdafx.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ReadWechatPay w;
	w.InIt();
	return a.exec();
}
