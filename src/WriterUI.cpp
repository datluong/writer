// Default empty project template
#include "WriterUI.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDebug>

using namespace bb::cascades;

WriterUI::WriterUI(bb::cascades::Application *app)
: QObject(app)
{
    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty( "writerApp", this);
    // create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    // set created root object as a scene
    app->setScene(root);

    // load data from ./documents folder

}

/**
 * Create an empty file and return the new path.
 * @return   empty string if the new document couldn't be created in the specified path
 */
QString WriterUI::createEmptyFile( QString documentPath ) {
	if (documentPath.size() > 0 && !documentPath.startsWith("/"))
			documentPath = "/" + documentPath;
	QString destPath = QDir::currentPath() + "/data" + documentPath;
	qDebug() << "WriterUI::createEmptyFile:path" << documentPath << "fullPath" << destPath;

	QDir dir(destPath);
	if (!dir.exists()) return "";

	int counter = 0;
	while (true) {
		QString fileName = untitledFilePath(destPath, counter);
		QFile file(fileName);
		if (!file.exists())
			break;
		counter ++;
	}

	QString newFileName = untitledFilePath(destPath, counter);
	QFile newFile( newFileName );
	if ( !newFile.open(QIODevice::WriteOnly) )
		return "";

	newFile.close();
	return newFileName;
}

QString WriterUI::untitledFilePath(const QString& path, int counter) {
	QString fileName = path + "/untitled";
	if (counter > 0)
		fileName += QString(" (%1)").arg(counter);
	fileName += ".txt";
	return fileName;
}

/**
 * List file in given directory and return a list of QVariantMap for QML controls
 */
QVariantList WriterUI::listDirectory(QString path) {
	qDebug() << "WriterUI::listDirectory:path:" << path;

	QVariantList docList;
	QString destPath = QDir::currentPath() + "/data";

	QDir dir(destPath);
	if (!dir.exists())
		return docList;

	QFileInfoList infoList = dir.entryInfoList( QDir::NoFilter, QDir::DirsFirst | QDir::Name );
	foreach (QFileInfo fileInfo, infoList) {
		qDebug() << "entry:name" << fileInfo.fileName() << "path:" << fileInfo.filePath() << "isDir:" << fileInfo.isDir() << "isFile:" << fileInfo.isFile();
		if ( fileInfo.fileName() == "." || fileInfo.fileName() == ".." ) continue;
		if ( fileInfo.isDir() ) {
			QVariantMap entry;
			entry["type"] = "folder";
			entry["name"] = fileInfo.fileName();
			entry["path"] = fileInfo.filePath();
			docList << entry;
			continue;
		}

		if (fileInfo.isFile() && fileInfo.suffix().toLower() == "txt") {
			QVariantMap entry;
			entry["type"] = "file";
			entry["name"] = fileInfo.baseName();
			entry["path"] = fileInfo.filePath();
			docList << entry;
			continue;
		}
	}

	return docList;
}


bool WriterUI::isFileLoadable(QString filePath) {
	QFile file(filePath);
	return file.exists();
}

QString WriterUI::loadFileContent(QString filePath) {
	QFile file(filePath);
	if (!file.exists())
		return "";

	if (!file.open( QIODevice::ReadOnly ))
		return "";
	QByteArray data = file.readAll();
	file.close();

	return QString::fromUtf8(data.constData());
}

/**
 * Save Document
 * @param   filePath           The origininal path of document
 * @param   documentTitle      Document's title. The saved file might be renamed if the title is changed.
 * @param   documentContent    Content of the document
 * @return                     0 if there is no error,
 *                             1 if there is no error, but the document name has been changed
 *                             or an non-zero error code
 */
int WriterUI::saveDocument(QString filePath, QString documentTitle, QString documentContent) {
	bool titleChanged = false;
	qDebug() << "WriterUI::saveDocument:filePath" << filePath << ":title:" << documentTitle;
	QFile file(filePath);

	if ( !file.exists() ) return 0x1000;
	// rename if needed
	QFileInfo fileInfo( file );
	qDebug() << "File basename:" << fileInfo.baseName() << "completeBaseName:" << fileInfo.completeBaseName();
	QString newFileName = correctFileName( documentTitle );
	if (newFileName != fileInfo.baseName()) {
		// rename the file
		QString newName = fileInfo.path() + "/" + newFileName + ".txt";
		qDebug() << "Update document's name -> " << newName;
		if (!file.rename(newName))
			return 0x1010;
		titleChanged = true;
	}
	// TODO

	if ( !file.open(QIODevice::WriteOnly) )
		return 0x1001;

	if ( file.write( documentContent.toUtf8() ) == -1 ) {
		file.close();
		return 0x1002;
	}

	file.close();
	return titleChanged?1:0;
}

/**
 * Fix windows/osx/linux filename by replacing forbidden characters with spaces
 */
QString WriterUI::correctFileName(const QString& fileName) {
	QString forbiddenChars = "\\:*?\"<>|/";
	QString name = fileName;
	for (int i = 0; i < forbiddenChars.size();i++) {
		QChar c = forbiddenChars.at(i);
		name = name.replace( c, '-' );
	}
	return name;
}
