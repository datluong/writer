// Default empty project template
#ifndef WriterUI_HPP_
#define WriterUI_HPP_

#include <QObject>
#include <QVariant>
#include <QVariantList>

namespace bb { namespace cascades { class Application; }}

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
    Q_INVOKABLE int saveDocument(QString filePath, QString documentTitle, QString documentContent);
    Q_INVOKABLE QString createEmptyFile( QString documentPath );
private:
    QString correctFileName(const QString& fileName);
    QString untitledFilePath(const QString& path, int counter);
};


#endif /* ApplicationUI_HPP_ */
