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
		// ��ȡ������
		QFileInfo fileInfo(QCoreApplication::applicationFilePath());
		QString appName = fileInfo.baseName();

		// ��ȡ��ǰ����ʱ��
		//QString currentTime = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
		QString currentTime = QDateTime::currentDateTime().toString("yyyyMMdd");

		// ȷ����������Ŀ¼�µ�log�ļ��д���
		QString logFolder = QDir::currentPath() + "/log";
		QDir().mkdir(logFolder);

		// ������־�ļ�ȫ·��, ��ʽΪ ${��������Ŀ¼}/log/${appName}_${currentTime}.log
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