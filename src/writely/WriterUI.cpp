// Default empty project template
#include "WriterUI.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <bb/device/HardwareInfo>

#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDebug>
#include <QMetaObject>
#include <QMetaMethod>
#include <QTimer>


using namespace bb::cascades;

namespace writely {

WriterUI::WriterUI(bb::cascades::Application *app)
: QObject(app)
{

	// filesystem initialization
	initializeDocumentFolder();
	qmlRegisterType<QTimer>("my.timer", 1, 0, "QTimer");

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
    connect( app, SIGNAL(thumbnail()),   this, SLOT(onAppThumbnailed()) );
    connect( app, SIGNAL(fullscreen()),  this, SLOT(onAppFullscreen()) );

    initializeAutosave();
}

void WriterUI::initializeDocumentFolder() {
	QString documentsPath = QDir::homePath() + "/documents";
	QDir dir(documentsPath);
	if (!dir.exists())
		dir.mkpath(documentsPath);
}

/**
 * Initilize Auto-save module
 */
void WriterUI::initializeAutosave() {
	QTimer* theTimer = new QTimer(this);
	theTimer->setInterval( 15000 ); // every 15 secs
	connect( theTimer, SIGNAL(timeout()), this, SLOT(onAutosaveTimerTimeout()) );
	theTimer->start();
}

/**
 * Create an empty folder and return the folder info
 * @param    documentPath    start with "/"
 */
QVariantMap WriterUI::createEmptyFolder( QString documentPath ) {
	QString destPath = documentsFolderPath() + documentPath;
	qDebug() << "WriterUI::createEmptyFolder" << ":documentPath:" << documentPath << "destPath:" << destPath;
	QDir dir(destPath);
	if (!dir.exists()) return QVariantMap();

	int counter = 0;
	while (true) {
		QString folderName = genFolderPath(destPath, counter);
		QDir newFolder(folderName);
		if (!newFolder.exists())
			break;
		counter ++;
	}

	QString newFolderName =  genFolderPath(destPath, counter);
	QDir newFolder( newFolderName );

	if (!newFolder.mkdir( newFolderName ))
		return QVariantMap();

	QFileInfo folderInfo( newFolderName );
	QVariantMap entry;
	entry["type"] = "folder";
	entry["name"] = folderInfo.baseName();
	entry["path"] = folderInfo.filePath();
	qDebug() << "Created New Folder:" << entry;
	return entry;
}

/**
 * Rename an folder and return the folder info.
 * An empty QVariant is returned if there's an error
 */
QVariantMap WriterUI::renameFolder( QString relativeFolderPath, QString newName ) {
	QString destPath = documentsFolderPath() + relativeFolderPath;
	qDebug() << "WriterUI::renameFolder" << destPath << " -> " << newName;
	QFileInfo folderInfo(destPath);
	QDir folder(destPath);
	if (!folder.exists()) return QVariantMap();
	QDir parentFolder(destPath);
	parentFolder.cdUp();
	qDebug() << "parentFolder" << parentFolder.path();

	int counter = 0;
	while (true) {
		QString folderName = genFolderPath(parentFolder.path(), counter, newName);
		QDir newFolder(folderName);
		if (!newFolder.exists())
			break;
		counter ++;
	}
	QString newFolderName = genFolderPath(parentFolder.path(), counter, newName);
	if ( !folder.rename( destPath, newFolderName ) )
		return QVariantMap();

	QFileInfo info(newFolderName);
	if ( !info.exists() )
		return QVariantMap();

	QVariantMap entry;
	entry["type"] = "folder";
	entry["name"] = info.baseName();
	entry["path"] = info.filePath();
	entry["relativePath"] = relativePath( info.filePath() );

	qDebug() << "WriterUI::renameFolder:reesult:" << entry;
	return entry;

}

/**
 * Create an empty file and return the new path.
 * @return   empty map if the new document couldn't be created in the specified path
 */
QVariantMap WriterUI::createEmptyFile( QString documentPath ) {
	if (documentPath.size() > 0 && !documentPath.startsWith("/"))
			documentPath = "/" + documentPath;
	QString destPath = QDir::homePath() + "/documents" + documentPath;
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

/**
 * return fullPath
 */
QString WriterUI::genFolderPath(QString path, int counter, QString defaultName ) {
	QString folderName = path + "/" + defaultName;
	if (counter > 0)
		folderName += QString(" %1").arg(counter);
	return folderName;
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

QString WriterUI::relativePath(QString filePath) {
	QString rootPath = QDir::homePath() + "/documents";
	QString relativePath = filePath.replace(rootPath,"");
	if (relativePath.isEmpty())
		relativePath = "/";
	return relativePath;
}

/**
 * List file in given directory and return a list of QVariantMap for QML controls
 */
QVariantList WriterUI::listDirectory(QString path) {
	qDebug() << "WriterUI::listDirectory:path:" << path;

	QVariantList docList;
	QString destPath = QDir::homePath() + "/documents";
	if (path.length() > 0)
		destPath += path;

	QDir dir(destPath);
	if (!dir.exists())
		return docList;

	QFileInfoList infoList = dir.entryInfoList( QDir::NoFilter, QDir::DirsFirst | QDir::Name  | QDir::IgnoreCase );
	foreach (QFileInfo fileInfo, infoList) {
//		qDebug() << "entry:name" << fileInfo.fileName() << "path:" << fileInfo.filePath() << "isDir:" << fileInfo.isDir() << "isFile:" << fileInfo.isFile();
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

/**
 * Recursive delete every files and subfolder in given folder, then delete the target folder
 * @param    folderPath   The full path of the folder to be deleted
 */
bool WriterUI::deleteFolder(QString folderPath) {
	qDebug() << "WriterUI::deleteFolder" << folderPath;
	// make sure the folder is a subfolder of ./documents folder
	if (folderPath.indexOf( documentsFolderPath() ) != 0) {
		qDebug() << "WriterUI::deleteFolder can't executed, folderPath is invalid";
		return false;
	}

	QDir theFolder(folderPath);
	if (!theFolder.exists()) return false;

	QList<QFileInfo> infoList = theFolder.entryInfoList();
	foreach (QFileInfo fileInfo, infoList) {
		if (fileInfo.fileName() == "." || fileInfo.fileName() == ".." ) continue;
		qDebug() << "deleting.." << fileInfo.fileName();
		if (fileInfo.isFile()) {
			bool status = QFile::remove(fileInfo.filePath());
			qDebug() << " -> delete status:" << status << ":" << fileInfo.filePath();
			if ( !status ) return false;
		}
		else if (fileInfo.isDir()) {
			bool status = this->deleteFolder(fileInfo.filePath());
			if ( !status ) return false;
			qDebug() << " -> delete status:" << status << ":" << fileInfo.filePath();
		}
	}

	return theFolder.rmdir(folderPath);
}

QString WriterUI::documentsFolderPath() {
	return QDir::homePath() + "/documents";
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

bool WriterUI::documentPathMathTitle(QString filePath, QString documentTitle) {
	QString newFileName = correctFileName( documentTitle );
	if (newFileName.isEmpty())
		return false;
	QFileInfo fileInfo(filePath);
	return fileInfo.baseName() == documentTitle;
}

/**
 * Save Document
 * @param   filePath           The origininal path of document
 * @param   documentTitle      Document's title. The saved file might be renamed if the title is changed.
 * @param   documentContent    Content of the document
 * @param   saveOptions        :keepTitle    Do not rename the file if title is changed
 * @return                     0 if there is no error,
 *                             1 if there is no error, but the document name has been changed
 *                             or an non-zero error code
 */
int WriterUI::saveDocument(QString filePath, QString documentTitle, QString documentContent, QVariantMap saveOptions) {
	bool keepTitle    = ( saveOptions.contains("keepTitle") && saveOptions["keepTitle"].toBool() );
	bool titleChanged = false;
	qDebug() << "WriterUI::saveDocument:filePath" << filePath << ":title:" << documentTitle<< ":saveOptions:" << saveOptions;
	QFile file(filePath);

	if ( !file.exists() ) return 0x1000;

	if (!keepTitle) {
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

/**
 * @return the editor page if it is on the top of navigation stack. Otherwise return 0;
 */
Page* WriterUI::currentEditorPage() {
	if (mRootNavigationPane->count() > 0) {
		Page* page = mRootNavigationPane->at( mRootNavigationPane->count()-1 );
		if (page) {
			if ( page->objectName() == "editorPage" ) {
				return page;
			}
		}
	}
	return NULL;
}

bool WriterUI::isPhysicalKeyboardDevice() {
	bb::device::HardwareInfo hwInfo;
	return hwInfo.isPhysicalKeyboardDevice();
}

void WriterUI::onAppAboutToQuit() {
	qDebug() << "WriterUI::onAppAboutToQuit()";
	Page* page = currentEditorPage();
	if (page)
		QMetaObject::invokeMethod(page, "handleAppExitEvent" );
}

//connect( app, SIGNAL(thumbnail()),   this, SLOT(onAppThumbnailed()) );
//connect( app, SIGNAL(fullscreen()),  this, SLOT(onAppFullscreen()) );

void WriterUI::onAppThumbnailed() {
	qDebug() << "WriterUI::onAppThumbnailed()";
	Page* page = currentEditorPage();
	if (page)
		QMetaObject::invokeMethod(page, "onThumbnailed" );
}

void WriterUI::onAppFullscreen() {
	qDebug() << "WriterUI::onAppFullscreen";
	Page* page = currentEditorPage();
	if (page)
		QMetaObject::invokeMethod(page, "onFullscreen" );
}

/**
 * Fire after a certain period of time
 */
void WriterUI::onAutosaveTimerTimeout() {
	Page* page = currentEditorPage();
	if (page)
		QMetaObject::invokeMethod(page, "handleAutoSaveEvent" );
}


} // end namespace
