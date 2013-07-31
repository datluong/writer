// Default empty project template
#include "WriterUI.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDebug>
#include <QMetaObject>
#include <QMetaMethod>


using namespace bb::cascades;

namespace writely {

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

    mRootNavigationPane = dynamic_cast<NavigationPane*>(root);
    qDebug() << "rootPane" << (mRootNavigationPane!=NULL);

    connect( app, SIGNAL(aboutToQuit()), this, SLOT(onAppAboutToQuit()) );
}

/**
 * Create an empty file and return the new path.
 * @return   empty map if the new document couldn't be created in the specified path
 */
QVariantMap WriterUI::createEmptyFile( QString documentPath ) {
	if (documentPath.size() > 0 && !documentPath.startsWith("/"))
			documentPath = "/" + documentPath;
	QString destPath = QDir::currentPath() + "/data" + documentPath;
	qDebug() << "WriterUI::createEmptyFile:path" << documentPath << "fullPath" << destPath;

	QDir dir(destPath);
	if (!dir.exists()) return QVariantMap();

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
		return QVariantMap();

	newFile.close();

	QFileInfo fileInfo( newFile );
	QVariantMap entry;
	entry["type"] = "file";
	entry["name"] = fileInfo.baseName();
	entry["path"] = fileInfo.filePath();
	return entry;
}

QString WriterUI::untitledFilePath(const QString& path, int counter) {
	QString fileName = path + "/Untitled";
	if (counter > 0)
		fileName += QString(" %1").arg(counter);
	fileName += ".txt";
	return fileName;
}

/**
 * Only return the name of available file
 * @param    path  path for generating file
 * @param    originalPath   A generated name match originalPath is acceptable, regardless the existence of the file with that name
 */
QString WriterUI::availableUntitledFilePath( const QString& path, const QString& originalPath ) {
	int counter = 0;
	while (true) {
		QString fileName = untitledFilePath(path, counter);
		if (fileName == originalPath) {
			QFileInfo info(originalPath);
			return info.baseName();
		}
		QFile file(fileName);
		if (!file.exists())
			break;
		counter ++;
	}
	QString fullFilePath = untitledFilePath("", counter);
	QFileInfo info(fullFilePath);
	return info.baseName();
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

	QFileInfoList infoList = dir.entryInfoList( QDir::NoFilter, QDir::DirsFirst | QDir::Name  | QDir::IgnoreCase );
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

bool WriterUI::deleteFile(QString filePath) {
	qDebug() << "WriterUI:deleteFile:" << filePath;

	QFile file(filePath);
	if (!file.exists()) return false;

	return file.remove();
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

	// if newFileName is empty, pick an unused name
	if (newFileName.isEmpty()) {
		newFileName = availableUntitledFilePath( fileInfo.path(), filePath );
	}

	if (newFileName != fileInfo.baseName()) {
		// rename the file
		QString newName = fileInfo.path() + "/" + newFileName + ".txt";
		if ( QFile::exists(newName) ) {
			// if there is already a file with given name, add a counter value to filename
			int counter = 0;
			while (true) {
				QString fName = fileInfo.path() + "/" + newFileName + ( counter==0?"":QString(" %1").arg(counter) ) + ".txt";
				if (!QFile::exists(fName)) break;
				counter++;
			}
			newName = fileInfo.path() + "/" + newFileName + ( counter==0?"":QString(" %1").arg(counter) ) + ".txt";
		}

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
	name = name.replace( "\n", "");
	return name.trimmed();
}

void dumpMetaObject(const QMetaObject* metaObject) {
	qDebug() << "dumpMetaObject" << metaObject;
	if (metaObject == NULL) return;
	qDebug() << "className:" << metaObject->className();
	qDebug() << "methodCount:" << metaObject->methodCount();
	for (int i = 0 ; i < metaObject->methodCount();i++) {
		QMetaMethod method = metaObject->method(i);
		qDebug() << "method #" << i << ":signature:" << method.signature() << ":returnType:" << method.typeName();
	}
}

void WriterUI::onAppAboutToQuit() {
	qDebug() << "WriterUI::onAppAboutToQuit()";
	qDebug() << "Pages Count:" << mRootNavigationPane->count();
	if (mRootNavigationPane->count() > 0) {
		Page* page = mRootNavigationPane->at( mRootNavigationPane->count()-1 );
		if (page) {
			if ( page->objectName() == "editorPage" ) {
				QMetaObject::invokeMethod(page, "handleAppExitEvent" );
			}
		}
	}
}

} // end namespace
