/*
 * ThemeManager.h
 *
 *  Created on: Aug 3, 2013
 *      Author: iMac
 */

#ifndef THEMEMANAGER_H_
#define THEMEMANAGER_H_


#include <QVariantMap>
#include <QString>
#include <QObject>

namespace writely {

class ThemeManager: public QObject {
	Q_OBJECT
public:
	ThemeManager();
	virtual ~ThemeManager();

	Q_INVOKABLE QVariantMap currentTheme();
	Q_INVOKABLE QString currentThemeName();
	Q_INVOKABLE void setTheme( QString themeName );

Q_SIGNALS:
	void themeChanged( QVariantMap newThemeInfo );

private:
	QString mCurrentThemeName;

	QString defaultThemeName();
};

} /* namespace writely */
#endif /* THEMEMANAGER_H_ */
