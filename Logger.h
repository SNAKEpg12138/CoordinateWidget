#pragma once

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>

class Logger {
public:

	static bool startLogService()
	{
		// 获取程序名
		QFileInfo fileInfo(QCoreApplication::applicationFilePath());
		QString appName = fileInfo.baseName();

		// 获取当前日期时间
		//QString currentTime = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
		QString currentTime = QDateTime::currentDateTime().toString("yyyyMMdd");

		// 确保程序运行目录下的log文件夹存在
		QString logFolder = QDir::currentPath() + "/log";
		QDir().mkdir(logFolder);

		// 构造日志文件全路径, 格式为 ${程序运行目录}/log/${appName}_${currentTime}.log
		QString fullLogFileName = QString("%1/%2_%3.log").arg(logFolder, appName, currentTime);

		m_logFile.setFileName(fullLogFileName);

		if (!m_logFile.open(QIODevice::Append | QIODevice::Text))
		{
			return false;
		}
		return true;
	}

	static void stopLogService() {
		if (m_logFile.isOpen()) {
			m_logFile.close();
		}
	}

	static void writeLog(const QString& message)
	{
		startLogService();
		if (!m_logFile.isOpen())
			return;


		QTextStream out(&m_logFile);
		out.setCodec("UTF-8");

		static bool bFirstRun = true;
		if (bFirstRun)
		{
			bFirstRun = false;
			out << "\n======software begin running======\n";
		}

		out << /*QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz ") << */message << "\n";
		m_logFile.flush();
		stopLogService();
	}

private:
	static QFile m_logFile;
};

QFile Logger::m_logFile;