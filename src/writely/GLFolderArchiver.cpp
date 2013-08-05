/*
 * GLFolderAcchiver.cpp
 *
 *  Created on: Aug 5, 2013
 *      Author: iMac
 */

#include "GLFolderArchiver.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>

#include "quazip.h"
#include "quazipfile.h"

#include <utime.h>

namespace writely {

GLFolderArchiver::GLFolderArchiver() {
}

GLFolderArchiver::~GLFolderArchiver() {
}

/**
 * Compress all text files in destined folder and perverse the folder structure
 */
int GLFolderArchiver::zipFolder(const QString &folderName, const QString& saveToPath) {
	qDebug() << "GLFolderAcchiver::zipFolder:" << folderName << ":saveTo:" << saveToPath;

	QuaZip archiveFile( saveToPath );
	if ( !archiveFile.open(QuaZip::mdCreate) ) {
		qDebug() << "Failed to create archive:" << saveToPath;
		return -1;
	}

	QStringList fileList;
	listTxtFiles( folderName, &fileList);

	for (int i =0;i<fileList.size();i++) {
		QString path = fileList[i];

		QString relativeName = path;
		relativeName.replace( folderName, "" );
		if (relativeName.length()>1 && relativeName.startsWith("/"))
			relativeName.remove(0,1);

		qDebug() << "Listing:" << path << ":relativePath:" << relativeName;
		QuaZipFile zipFile( &archiveFile );
		QuaZipNewInfo newInfo(relativeName);
		newInfo.setFileDateTime(path); // perverse timestamp value

		bool openStatus = zipFile.open( QIODevice::WriteOnly | QIODevice::Text, newInfo);
		if (openStatus) {
			QFile file(path);
			file.open(QIODevice::ReadOnly | QIODevice::Text);
			QByteArray ba = file.readAll();
			zipFile.write(ba);
			file.close();
		}
		zipFile.close();
	}

	archiveFile.close();
	return 0;
}

void GLFolderArchiver::listTxtFiles( const QString& folderName, QStringList* resultList ) {
	QDir folder(folderName);
	if (!folder.exists()) return;

	QFileInfoList fileList = folder.entryInfoList();

	for (int i = 0 ; i < fileList.size();i++) {
		QFileInfo info = fileList[i];
		if (info.fileName() == "." || info.fileName() == ".." ) continue;
//		qDebug() << "#" << i << ":" << info.filePath();
		if (info.isFile()) {
			if (info.fileName().endsWith(".txt"))
				resultList->append( info.filePath() );
		}
		else if (info.isDir()) {
			listTxtFiles(info.filePath(), resultList);
		}
	}
}

/**
 * Extract files in archive to destinated folder
 */
void GLFolderArchiver::unarchiveTxtFiles( const QString& archiveFilePath, const QString& extractToFolder) {
	QDir extractDir(extractToFolder);
	if (!extractDir.exists())
		extractDir.mkpath(extractToFolder);

	if (!QFile::exists(archiveFilePath)) return;

	QuaZip archive( archiveFilePath );
	if (!archive.open(QuaZip::mdUnzip)) {
		return;
	}

	QStringList fileList = archive.getFileNameList();
	for (int i = 0; i < fileList.size(); i++) {
		QString filePath = fileList[i];
		QString fullFilePath = extractToFolder + "/" + filePath;
		QFileInfo fileInfo(fullFilePath);
		qDebug() << "unarchiveTxtFiles::write:" << fullFilePath;

		// make path to file
		QDir destDir( fileInfo.path() );
		if ( !destDir.exists() ) {
			bool pathResult = destDir.mkpath( fileInfo.path() );
			qDebug() << "unarchiveTxtFiles::createPath:" << fileInfo.path() << ":result:" << pathResult;
			if (!pathResult) continue;
		}

		// read from zip File
		QuaZipFile zipFile( &archive );
//		qDebug() << "zipFileInfo:name" << zipFileInfo.name << "time:" << zipFileInfo.dateTime;

		archive.setCurrentFile( filePath );

		QuaZipFileInfo zipFileInfo;
		archive.getCurrentFileInfo( &zipFileInfo );

		// read from source
		if ( !zipFile.open(QIODevice::ReadOnly) )
			continue;
		QByteArray content = zipFile.readAll();
		zipFile.close();

		// write to destination file
		QFile outFile( fullFilePath );
		if ( !outFile.open( QIODevice::WriteOnly | QIODevice::Text) ) {
			continue;
		}
		outFile.write( content );
		outFile.close();

		// update timestamp
		// @non-portable
		int ts = zipFileInfo.dateTime.toMSecsSinceEpoch() / 1000L;
		struct utimbuf modTime;
		modTime.modtime = ts;
		int utimeStatus = utime( fullFilePath.toLocal8Bit(), &modTime );
	}
}

/**
 * Count the number of .txt files in the archive
 * @return a negative number if Read Error
 */
int GLFolderArchiver::countTxtFiles( const QString& archiveFilePath ) {
	if (!QFile::exists(archiveFilePath)) return -1;

	QuaZip archive( archiveFilePath );
	if (!archive.open(QuaZip::mdUnzip)) {
		return -2;
	}

	int count = 0;

	QStringList fileList = archive.getFileNameList();
	foreach (QString fName, fileList) {
		// fileName with path will look like this: "Misc/Start writing.txt"
		if (fName.endsWith(".txt")) count++;
	}

	archive.close();

	return count;
}

} /* namespace writely */
