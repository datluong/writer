/*
 * ThemeManager.h
 *
 *  Created on: Aug 3, 2013
 *      Author: iMac
 */

#ifndef THEMEMANAGER_H_
#define THEMEMANAGER_H_


#include <QVariantMap>
#include <QVariantList>
#include <QString>
#include <QObject>
#include <QStringList>

namespace writely {

class ThemeManager: public QObject {
	Q_OBJECT
public:
	ThemeManager();
	virtual ~ThemeManager();

	Q_INVOKABLE QVariantMap currentTheme();
	Q_INVOKABLE QString defaultThemeName();
	Q_INVOKABLE QString currentThemeName();
	Q_INVOKABLE void setTheme( QString themeName );
	Q_INVOKABLE QVariantList themeList();
Q_SIGNALS:
	void themeChanged( QVariantMap newThemeInfo );

private:
	QString mCurrentThemeName;
	QStringList mThemeNameList;
	QVariantMap mThemeDictionary;


	void initializeThemes();

	void saveThemeToSettings( const QString themeName );
	void loadThemeFromSettings();
};

} /* namespace writely */
#endif /* THEMEMANAGER_H_ */
