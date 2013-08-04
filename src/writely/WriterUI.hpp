// Default empty project template
#ifndef WriterUI_HPP_
#define WriterUI_HPP_

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QSize>

#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/UIOrientation>

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
private:
    NavigationPane* mRootNavigationPane;
    QVariantMap mEmbeddedData;
    InvokeQueryTargetsReply * _queryResults;
    bb::system::SystemToast* mToast;
    bool mIsThumbnail;
    QSize* mDisplaySize;
    ThemeManager* mThemeManager;

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

private slots:
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
};

}

#endif /* ApplicationUI_HPP_ */
