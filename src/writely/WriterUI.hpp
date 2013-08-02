// Default empty project template
#ifndef WriterUI_HPP_
#define WriterUI_HPP_

#include <QObject>
#include <QVariant>
#include <QVariantList>

#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>

using namespace bb::cascades;

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
    Q_INVOKABLE int saveDocument(QString filePath, QString documentTitle, QString documentContent, QVariantMap saveOptions);
    Q_INVOKABLE bool documentPathMathTitle(QString filePath, QString documentTitle);

    Q_INVOKABLE QVariantMap createEmptyFolder( QString documentPath );
    Q_INVOKABLE QVariantMap createEmptyFile( QString documentPath );
    Q_INVOKABLE bool deleteFile(QString filePath);
    Q_INVOKABLE bool deleteFolder(QString folderPath);
    Q_INVOKABLE QVariantMap renameFolder( QString relativeFolderPath, QString newName );
    Q_INVOKABLE QString relativePath(QString filePath);
    Q_INVOKABLE bool isPhysicalKeyboardDevice();
    Q_INVOKABLE QString correctFileName(const QString& fileName);
private:
    NavigationPane* mRootNavigationPane;

    QString untitledFilePath(const QString& path, int counter);
    QString genFolderPath(QString path, int counter, QString defaultName = QString("Untitled Folder") );


    QString availableUntitledFilePath( const QString& path,  const QString& originalPath );
    QString documentsFolderPath();

    void initializeDocumentFolder();
    void initializeAutosave();

    Page* currentEditorPage();

private slots:
	void onAppAboutToQuit();
	void onAutosaveTimerTimeout();
	void onAppThumbnailed();
	void onAppFullscreen();

};

}

#endif /* ApplicationUI_HPP_ */
