// Default empty project template
#include "WriterUI.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/OrientationSupport>
#include <bb/cascades/Sheet>
#include <bb/cascades/ArrayDataModel>
#include <bb/cascades/pickers/FilePicker>

#include <bb/device/HardwareInfo>
#include <bb/device/DisplayInfo>

#include <bb/PpsObject>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeQueryTargetsRequest>
#include <bb/system/InvokeQueryTargetsReply>
#include <bb/system/InvokeReplyError>
#include <bb/system/SystemDialog>


#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDebug>
#include <QMetaObject>
#include <QMetaMethod>
#include <QTimer>

using namespace bb::cascades;
using namespace bb::system;
using namespace bb::cascades::pickers;

namespace writely {

WriterUI::WriterUI(bb::cascades::Application *app)
: QObject(app)
{
	// private vars initialization
	mEmbeddedData = QVariantMap();
	_queryResults = NULL;
	mToast        = NULL;

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

///////////////////////////////////////////////////////////////////////////////
// SHARING SUPPORT
///////////////////////////////////////////////////////////////////////////////

void WriterUI::actionShareDocumentAsAttachment(QString title, QString body) {
	InvokeManager invokeManager;
	InvokeQueryTargetsRequest request;

	mEmbeddedData["shareTitle"] = title;
	mEmbeddedData["shareBody"]  = body;

	request.setAction( "bb.action.SHARE" );
	request.setMimeType( "*" );
	request.setUri( QUrl("file://" + QDir::currentPath() + "/shared/documents/sample.txt") );
	InvokeQueryTargetsReply* results = invokeManager.queryTargets(request);

	if (!results) {
		qWarning() << "Can't query MIME type";
	}
	else {
		results->setParent(this);
		connect(results, SIGNAL(finished()), this, SLOT(onWildcardQueryResponse()));
		_queryResults = results;
	}
}

void WriterUI::onWildcardQueryResponse() {
	qDebug() << "WriterUI::onWildcardQueryResponse()";
	if (_queryResults == NULL) return;

	if (_queryResults->error() != InvokeReplyError::None) {
		qDebug() << "InvokeReplyError:" << _queryResults->error();
		return;
	}
	if (_queryResults->actions().size() == 0) return;
	InvokeAction action = _queryResults->actions().first();
	if (action.targets().size() == 0) return;
	qDebug() << "Label" << action.label();
	// load the sheet
	QmlDocument* qml = QmlDocument::create("asset:///ShareTargetPickerSheet.qml");
	Sheet* sheet     = qml->createRootObject<Sheet>();
	ArrayDataModel* dataModel = sheet->findChild<ArrayDataModel*>("shareModels");
	qDebug() << "dataModel" << (dataModel!=NULL?true:false);
	if (dataModel) {
		foreach (InvokeTarget target, action.targets()) {
			QVariantMap map;
			map["title"]       = target.label();
			map["imageSource"] = QUrl( QString("file://") + target.icon().toString() );
			map["target"]      = target.name();
			map["action"]      = action.name();
			dataModel->append(map);
		}
	}
	connect( sheet, SIGNAL(shareTargetPicked(QVariant)), this, SLOT(onWildcardShareTargetPicked(QVariant)) );
	sheet->open();

	delete _queryResults;
	_queryResults = NULL;
}

QString WriterUI::exportToTempDir() {
	// export the file to .slicktasks
	QString fileName = correctFileName(mEmbeddedData["shareTitle"].toString().trimmed());
	if (fileName.isEmpty()) fileName = "Untitled";
	fileName += ".txt";

	// create folder .slicktasks folder
	QDir tmpDir(QDir::currentPath() + "/shared/documents/.writer_tmp");

	if (!tmpDir.exists()) {
		qDebug() << "Temporary Directory does not exist. Creating.." << tmpDir.absolutePath();
		if (!tmpDir.mkdir( tmpDir.absolutePath() )) {
			qDebug() << "Can't create " << tmpDir.absolutePath();
			return "";
		}
	}

	QString filePath = tmpDir.absolutePath() + "/" + fileName;

	QFile file(filePath);
	if (file.open(QIODevice::WriteOnly)) {
		file.write( mEmbeddedData["shareBody"].toString().toUtf8() );
		file.close();
	}
	else {
		return "";
	}

	return filePath;
}

void WriterUI::cleanTemporarySharedFolder() {
	QDir tmpDir( QDir::currentPath() + "/shared/documents/.writer_tmp" );
	if (!tmpDir.exists()) return;

	QStringList fileList = tmpDir.entryList( QStringList() << "*" );
	foreach (QString fileName, fileList) {
		if (fileName == "." || fileName == "..") continue;
		QFile f(tmpDir.absolutePath() + "/" + fileName);
		f.remove();
	}
	tmpDir.rmdir(tmpDir.absolutePath());
}


void WriterUI::onWildcardShareTargetPicked(QVariant target) {
	qDebug() << "WriterUI::onWildcardShareTargetPicked:" << target;
	if (!target.canConvert(QVariant::Map)) return;

	QString filePath = exportToTempDir();
	if (filePath.isEmpty())
		return;
	// invoke to share
	QVariantMap map = target.toMap();
	InvokeManager invokeManager;
	InvokeRequest request;

	request.setAction( map["action"].toString() );
	request.setTarget( map["target"].toString() );
	request.setMimeType( "*" );
	request.setFileTransferMode( FileTransferMode::Preserve );
	request.setUri( QUrl( "file://" + filePath ) );

	invokeManager.invoke( request );
}




void WriterUI::actionShareDocument(QString title, QString body) {
	qDebug() << "WriterUI::actionShareDocument";
	InvokeManager invokeManager;
	InvokeQueryTargetsRequest request;

	request.setAction( "bb.action.SHARE" );
	request.setMimeType( "text/plain" );

	mEmbeddedData["shareTitle"] = title;
	mEmbeddedData["shareBody"]  = body;

	InvokeQueryTargetsReply* results = invokeManager.queryTargets(request);
	if (!results) {
		qWarning() << "Can't query MIME type";
	}
	else {
		results->setParent(this);
		connect(results, SIGNAL(finished()), this, SLOT(onTextQueryResponse()));
		_queryResults = results;
	}
}

void WriterUI::onTextQueryResponse() {
	qDebug() << "WriterUI::onTextQueryResponse()";

	if (_queryResults == NULL) return;
	if (_queryResults->error() != InvokeReplyError::None) {
		qDebug() << "InvokeReplyError:" << _queryResults->error();
		return;
	}

	if (_queryResults->actions().size() == 0) return;
	InvokeAction action = _queryResults->actions().first();
	if (action.targets().size() == 0) return;

	// load the sheet
	QmlDocument* qml = QmlDocument::create("asset:///ShareTargetPickerSheet.qml");
	Sheet* sheet     = qml->createRootObject<Sheet>();
	ArrayDataModel* dataModel = sheet->findChild<ArrayDataModel*>("shareModels");
	qDebug() << "dataModel" << (dataModel!=NULL?true:false);
	if (dataModel) {
		foreach (InvokeTarget target, action.targets()) {
			QVariantMap map;
			map["title"]       = target.label();
			map["imageSource"] = QUrl( QString("file://") + target.icon().toString() );
			map["target"]      = target.name();
			map["action"]      = action.name();
			dataModel->append(map);
		}
	}
	connect( sheet, SIGNAL(shareTargetPicked(QVariant)), this, SLOT(onTextShareTargetPicked(QVariant)) );
	sheet->open();

	delete _queryResults;
	_queryResults = NULL;
};


void WriterUI::onTextShareTargetPicked(QVariant target) {
	qDebug() << "WriterUI::onTextShareTargetPicked:" << target;
	if (!target.canConvert(QVariant::Map)) return;
	if (!mEmbeddedData.contains("shareBody")) return;

	QVariantMap map = target.toMap();
	if (!map.contains("target")) return;
	QString targetName	 = map["target"].toString();

	InvokeManager invokeManager;
	InvokeRequest request;

	if (targetName == "sys.pim.remember.composer") {
		// custom code for sharing title to remember
		request.setAction( "bb.action.ADD" );
		request.setTarget( "sys.pim.remember.composer" );

		QUrl uri( "remember://notebookentry" );
		uri.addEncodedQueryItem( QString("title").toAscii(), QUrl::toPercentEncoding( mEmbeddedData["shareTitle"].toString() ) );
		uri.addEncodedQueryItem( QString("description").toAscii(), QUrl::toPercentEncoding( mEmbeddedData["shareBody"].toString() ) );
		request.setUri( uri );

		invokeManager.invoke( request );
		return;
	}

	if (targetName == "sys.pim.uib.email.hybridcomposer" ) {
		request.setTarget( "sys.pim.uib.email.hybridcomposer" );
		request.setAction( "bb.action.COMPOSE" );
		request.setMimeType( "message/rfc822" );

		// prepare body
		QVariantMap map;
		map["subject"] = mEmbeddedData["shareTitle"].toString();
		map["body"]    = mEmbeddedData["shareBody"].toString();

		QVariantMap moreData;
		moreData["data"] = map;
		bool ok=true;
		request.setData( bb::PpsObject::encode(moreData, &ok) );

		invokeManager.invoke(request);
		return;
	}

	request.setAction( map["action"].toString() );
	request.setTarget( map["target"].toString() );
	request.setData( mEmbeddedData["shareBody"].toString().toUtf8() );
	request.setMimeType( "text/plain" );

	invokeManager.invoke( request );
}


void WriterUI::actionSaveToSharedFolder(QString title, QString body) {
	QString fileName = title.trimmed();
	if (fileName.isEmpty()) fileName = "Untitled";
	fileName += ".txt";

	mEmbeddedData["shareTitle"] = title;
	mEmbeddedData["shareBody"]  = body;

	FilePicker* picker = new FilePicker();
	picker->setMode( FilePickerMode::Saver );
	picker->setViewMode( FilePickerViewMode::ListView );
	picker->setDefaultSaveFileNames( QStringList() << fileName );
	connect( picker, SIGNAL(fileSelected(const QStringList&)),
			 this, SLOT(onFileSelectedForSaveAsTxt(const QStringList&)) );

	picker->open();

}

void WriterUI::onFileSelectedForSaveAsTxt(const QStringList& selectedFiles) {
	if (selectedFiles.size() == 0 ) return;
	if (!mEmbeddedData.contains("shareBody")) return;

	qDebug() << "WriterUI::onFileSelectedForSaveAsTxt" << selectedFiles;

	QString fileName = selectedFiles.first();

	QFile file(fileName);

	if (file.open(QIODevice::WriteOnly)) {
		file.write( mEmbeddedData["shareBody"].toString().toUtf8() );
		file.close();
		showToasts("Saved.");
	}
	else {
		showToasts("An error has occurred");
	}
}

// End of Sharing Section /////////////////////////////////////////////////////

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

void WriterUI::showToasts(const QString& message) {
    if (mToast == NULL)
        mToast = new bb::system::SystemToast( this );
    mToast->setBody( message );
    mToast->show();
}

bool WriterUI::isPhysicalKeyboardDevice2() {
	bb::device::HardwareInfo hwInfo;
	return hwInfo.isPhysicalKeyboardDevice();
}

bool WriterUI::isPhysicalKeyboardDevice() {
	bb::device::HardwareInfo hwInfo;
	return hwInfo.isPhysicalKeyboardDevice();
	if (hwInfo.isPhysicalKeyboardDevice() == false)
		return false;
	// hwInfo return true on simulator
	bb::device::DisplayInfo info( bb::device::DisplayInfo::primaryDisplayId() );
	QSize size = info.pixelSize();
	return size.width() == size.height();
}

/**
 * Return true if virtual keyboard is defined
 */
bool WriterUI::determineVirtualKeyboardShown(int screenWidth, int screenHeight) {
	Q_UNUSED(screenWidth);
	if (isPhysicalKeyboardDevice()) return false;

	bool isPortrait = OrientationSupport::instance()->orientation() == UIOrientation::Portrait;

	bb::device::DisplayInfo info( bb::device::DisplayInfo::primaryDisplayId() );
	QSize size = info.pixelSize();

	int fullHeight = isPortrait ? std::max(size.width(), size.height()) : std::min(size.width(), size.height());


	return (fullHeight - screenHeight) > 250;
}

void WriterUI::onAppAboutToQuit() {
	qDebug() << "WriterUI::onAppAboutToQuit()";
	Page* page = currentEditorPage();
	if (page) {
		QMetaObject::invokeMethod(page, "handleAppExitEvent" );
	}

	cleanTemporarySharedFolder();
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
