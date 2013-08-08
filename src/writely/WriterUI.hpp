// Default empty project template
#ifndef WriterUI_HPP_
#define WriterUI_HPP_

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QSize>
#include <QStringList>

#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/UIOrientation>

#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeQueryTargetsReply>
#include <bb/system/SystemToast>

#include "ThemeManager.h"

using namespace bb::cascades;
using namespace bb::system;

namespace bb { namespace cascades { class Application; }}

namespace writely {

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class WriterUI : public QObject
{
    Q_OBJECT
public:
    WriterUI(bb::cascades::Application *app);
    virtual ~WriterUI() {}
    Q_INVOKABLE QString appName();
    Q_INVOKABLE QString appVersion();
    Q_INVOKABLE void actionSendFeedback();
    Q_INVOKABLE void actionOpenBBW(QString appId);

    Q_INVOKABLE void registerClipboard( QString relativePath, QStringList fileList );
    Q_INVOKABLE void clearClipboard();
    Q_INVOKABLE bool isClipboardEmpty();
    Q_INVOKABLE QString clipboardRelativePath();
    Q_INVOKABLE QString clipboardDescription();
    Q_INVOKABLE void moveToFolder( QString relativePath );

    Q_INVOKABLE QVariantList listDirectory(QString path);
    Q_INVOKABLE bool isFileLoadable(QString filePath);
    Q_INVOKABLE QString loadFileContent(QString filePath);
    Q_INVOKABLE QVariantMap getFileInfo(QString filePath);
    Q_INVOKABLE int saveDocument(QString filePath, QString documentTitle, QString documentContent, QVariantMap saveOptions);
    Q_INVOKABLE bool documentPathMathTitle(QString filePath, QString documentTitle);

    Q_INVOKABLE QVariantMap createEmptyFolder( QString documentPath );
    Q_INVOKABLE QVariantMap createEmptyFile( QString documentPath );
    Q_INVOKABLE bool deleteFile(QString filePath);
    Q_INVOKABLE bool deleteFolder(QString folderPath);
    Q_INVOKABLE QVariantMap renameFolder( QString relativeFolderPath, QString newName );
    Q_INVOKABLE QString relativePath(QString filePath);
    Q_INVOKABLE bool isPhysicalKeyboardDevice();
    Q_INVOKABLE bool isPhysicalKeyboardDevice2();
    Q_INVOKABLE QString correctFileName(const QString& fileName);

    Q_INVOKABLE bool determineVirtualKeyboardShown(int screenWidth, int screenHeight);

    Q_INVOKABLE void actionShareDocument(QString title, QString body);
    Q_INVOKABLE void actionShareDocumentAsAttachment(QString title, QString body);
    Q_INVOKABLE void actionSaveToSharedFolder(QString title, QString body);

    Q_INVOKABLE void registerDocumentInEditing(QString filePath);
    Q_INVOKABLE QVariantMap lastDocumentInEditing();
    Q_INVOKABLE void unRegisterDocumentInEditing();

    Q_INVOKABLE int displayWidthForCurrentOrientation();

    Q_INVOKABLE void actionBackup();
    Q_INVOKABLE void actionRestore();

    Q_INVOKABLE QVariantMap invokeOptions();

    Q_INVOKABLE float editorFontSize();
    Q_INVOKABLE void  setEditorFontSize( float size );
    Q_INVOKABLE QString browserSortType();
    Q_INVOKABLE void  setBrowserSortType(QString sortType);
private:
    NavigationPane* mRootNavigationPane;
    QVariantMap mEmbeddedData;
    QVariantMap mBrowserClipboard;
    InvokeQueryTargetsReply * _queryResults;
    bb::system::SystemToast* mToast;
    bool mIsThumbnail;
    QSize* mDisplaySize;
    ThemeManager* mThemeManager;
    InvokeManager*  mInvokeManager;

    QString untitledFilePath(const QString& path, int counter);
    QString genFolderPath(QString path, int counter, QString defaultName = QString("Untitled Folder") );


    QString availableUntitledFilePath( const QString& path,  const QString& originalPath );
    QString documentsFolderPath();

    void initializeDocumentFolder();
    void initializeAutosave();

    Page* currentEditorPage();
    Page* currentBrowserPage();
    void showToasts( const QString& message );
    QString exportToTempDir();
    void cleanTemporarySharedFolder();
    void loadFileInfo( QVariantMap& map, const QFileInfo& fileInfo );

    // orientation
    UIOrientation::Type currentOrientation();
    int displayWidth(UIOrientation::Type type);

    // settings
    bool isWelcomeFileShown();
    void setWelcomeFileShown();
    void welcomeOnFirstTime();

    void broadcastClipboardChanged();

private slots:
	void handleInvoke(const bb::system::InvokeRequest& request);

	void onAppAboutToQuit();
	void onAutosaveTimerTimeout();
	void onAppThumbnailed();
	void onAppFullscreen();

	// sharing slots
	void onTextQueryResponse();
	void onTextShareTargetPicked(QVariant target);
	void onFileSelectedForSaveAsTxt(const QStringList& selectedFiles);
	void onWildcardQueryResponse();
	void onWildcardShareTargetPicked(QVariant target);

	void onThemeChanged(QVariantMap themeInfo);
	void onFileSelectedForBackup(const QStringList& selectedFiles);
	void onBackupFileSelectedForRestoring(const QStringList& selecteDFiles);
	void onRestoreConfirmationDialogFinished(bb::system::SystemUiResult::Type resultType);

};

}

#endif /* ApplicationUI_HPP_ */
