/*
 * ThemeManager.cpp
 *
 *  Created on: Aug 3, 2013
 *      Author: iMac
 */

#include "ThemeManager.h"
#include "GLConstants.h"

#include <QSettings>
#include <QStringList>

namespace writely {

ThemeManager::ThemeManager() {
	initializeThemes();
	mCurrentThemeName = defaultThemeName();
	loadThemeFromSettings();
}

ThemeManager::~ThemeManager() {
}

void ThemeManager::saveThemeToSettings( const QString themeName ) {
	if (!mThemeNameList.contains(themeName))
		return;

	QSettings settings(kGLCompanyName, kGLAppName);
	settings.setValue( "theme" , themeName );
}


void ThemeManager::loadThemeFromSettings() {
	QSettings settings(kGLCompanyName, kGLAppName);
	if (settings.contains( "theme" )) {
		QString themeName = settings.value( "theme" ).toString();
		if (mThemeNameList.contains(themeName))
			mCurrentThemeName = themeName;
	}
}

void ThemeManager::initializeThemes() {
	mThemeDictionary = QVariantMap();

	// Light theme initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]              = "Light";
		theme[kGLThemeKeyTextColor]         = "#000000";
		theme[kGLThemeKeyBackgroundColor]   = "#ffffff";
		theme[kGLThemeKeyDividerColor]      = "#e4e4e4";
		theme[kGLThemeKeyRowHighlightColor] = "#ebebeb";
		theme[kGLThemeKeyTitleDividerColor] = "#7f7f7f";
		theme[kGLThemeKeyType]              = "light";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Dark theme initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Dark";
		theme[kGLThemeKeyDividerColor]          = "#404040";
		theme[kGLThemeKeyTitleDividerColor]     = "#7f7f7f";
		theme[kGLThemeKeyBackgroundColor]       = "#000000";
		theme[kGLThemeKeyTextColor]             = "#ffffff";
		theme[kGLThemeKeyRowHighlightColor]     = "#a1a1a1";
		theme[kGLThemeWordCountBackgroundColor] = "#99ffffff";
		theme[kGLThemeWordCountTextColor]       = "#000000";
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_dark.png";
		theme[kGLThemeKeyType]                  = "dark";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Sepia theme initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Sepia";
		theme[kGLThemeKeyTextColor]             = "#57442e";//
		theme[kGLThemeKeyBackgroundColor]       = "#fbf0d9";//
		theme[kGLThemeKeyDividerColor]          = "#5557442e";
		theme[kGLThemeKeyTitleDividerColor]     = "#9957442e";
		theme[kGLThemeKeyRowHighlightColor]     = "#1a57442e";
		theme[kGLThemeWordCountBackgroundColor] = "#9957442e";
		theme[kGLThemeWordCountTextColor]       = "#fbf0d9";
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_sepia.png";
		theme[kGLThemeKeyType]                  = "light";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Zenburn Theme Initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Zenburn";
		theme[kGLThemeKeyTextColor]             = "#dcdccc";//
		theme[kGLThemeKeyBackgroundColor]       = "#3f3f3f";//
		theme[kGLThemeKeyDividerColor]          = "#55dcdccc"; // text
		theme[kGLThemeKeyTitleDividerColor]     = "#99dcdccc"; // text
		theme[kGLThemeKeyRowHighlightColor]     = "#1adcdccc"; // text
		theme[kGLThemeWordCountBackgroundColor] = "#99dcdccc"; // text
		theme[kGLThemeWordCountTextColor]       = "#3f3f3f";   // background
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_zenburn.png";
		theme[kGLThemeKeyType]                  = "dark";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Solarized Theme Initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Solarized Light";
		theme[kGLThemeKeyTextColor]             = "#657b83";//
		theme[kGLThemeKeyBackgroundColor]       = "#eee8d5";//
		theme[kGLThemeKeyDividerColor]          = "#55657b83"; // text
		theme[kGLThemeKeyTitleDividerColor]     = "#99657b83"; // text
		theme[kGLThemeKeyRowHighlightColor]     = "#1a657b83"; // text
		theme[kGLThemeWordCountBackgroundColor] = "#99657b83"; // text
		theme[kGLThemeWordCountTextColor]       = "#eee8d5";   // background
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_solarized_light.png";
		theme[kGLThemeKeyType]                  = "light";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Solarized Dark  Theme Initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Solarized Dark";
		theme[kGLThemeKeyTextColor]             = "#93a1a1";//
		theme[kGLThemeKeyBackgroundColor]       = "#002b36";//
		theme[kGLThemeKeyDividerColor]          = "#5593a1a1"; // text
		theme[kGLThemeKeyTitleDividerColor]     = "#9993a1a1"; // text
		theme[kGLThemeKeyRowHighlightColor]     = "#1a93a1a1"; // text
		theme[kGLThemeWordCountBackgroundColor] = "#9993a1a1"; // text
		theme[kGLThemeWordCountTextColor]       = "#002b36";   // background
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_solarized_dark.png";
		theme[kGLThemeKeyType]                  = "dark";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Spring Theme Initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Spring";
		theme[kGLThemeKeyTextColor]             = "#000000";//
		theme[kGLThemeKeyBackgroundColor]       = "#cce8cf";//
		theme[kGLThemeKeyDividerColor]          = "#55000000"; // text
		theme[kGLThemeKeyTitleDividerColor]     = "#99000000"; // text
		theme[kGLThemeKeyRowHighlightColor]     = "#1a000000"; // text
		theme[kGLThemeWordCountBackgroundColor] = "#99000000"; // text
		theme[kGLThemeWordCountTextColor]       = "#cce8cf";   // background
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_spring.png";
		theme[kGLThemeKeyType]                  = "light";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Chocolate Theme Initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Chocolate";
		theme[kGLThemeKeyTextColor]             = "#ffefd5";//
		theme[kGLThemeKeyBackgroundColor]       = "#3f1f1f";//
		theme[kGLThemeKeyDividerColor]          = "#55ffefd5"; // text
		theme[kGLThemeKeyTitleDividerColor]     = "#99ffefd5"; // text
		theme[kGLThemeKeyRowHighlightColor]     = "#1affefd5"; // text
		theme[kGLThemeWordCountBackgroundColor] = "#99ffefd5"; // text
		theme[kGLThemeWordCountTextColor]       = "#3f1f1f";   // background
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_chocolate.png";
		theme[kGLThemeKeyType]                  = "dark";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Breeze  Theme Initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Breeze";
		theme[kGLThemeKeyTextColor]             = "#ffffff";//
		theme[kGLThemeKeyBackgroundColor]       = "#005c70";//
		theme[kGLThemeKeyDividerColor]          = "#55ffffff"; // text
		theme[kGLThemeKeyTitleDividerColor]     = "#99ffffff"; // text
		theme[kGLThemeKeyRowHighlightColor]     = "#1affffff"; // text
		theme[kGLThemeWordCountBackgroundColor] = "#99ffffff"; // text
		theme[kGLThemeWordCountTextColor]       = "#005c70";   // background
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_breeze.png";
		theme[kGLThemeKeyType]                  = "light";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Matrix Theme Initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Matrix";
		theme[kGLThemeKeyTextColor]             = "#00f000";//
		theme[kGLThemeKeyBackgroundColor]       = "#000000";//
		theme[kGLThemeKeyDividerColor]          = "#5500f000"; // text
		theme[kGLThemeKeyTitleDividerColor]     = "#9900f000"; // text
		theme[kGLThemeKeyRowHighlightColor]     = "#1a00f000"; // text
		theme[kGLThemeWordCountBackgroundColor] = "#9900f000"; // text
		theme[kGLThemeWordCountTextColor]       = "#000000";   // background
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_matrix.png";
		theme[kGLThemeKeyType]                  = "dark";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Autumn Theme Initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Autumn";
		theme[kGLThemeKeyTextColor]             = "#404040";//
		theme[kGLThemeKeyBackgroundColor]       = "#fff4e8";//
		theme[kGLThemeKeyDividerColor]          = "#55404040"; // text
		theme[kGLThemeKeyTitleDividerColor]     = "#99404040"; // text
		theme[kGLThemeKeyRowHighlightColor]     = "#1a404040"; // text
		theme[kGLThemeWordCountBackgroundColor] = "#99404040"; // text
		theme[kGLThemeWordCountTextColor]       = "#fff4e8";   // background
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_autumn.png";
		theme[kGLThemeKeyType]                  = "light";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	// Tango  Theme Initialization
	{
		QVariantMap theme;
		theme[kGLThemeKeyName]                  = "Tango";
		theme[kGLThemeKeyTextColor]             = "#eeeeec";//
		theme[kGLThemeKeyBackgroundColor]       = "#2e3436";//
		theme[kGLThemeKeyDividerColor]          = "#55eeeeec"; // text
		theme[kGLThemeKeyTitleDividerColor]     = "#99eeeeec"; // text
		theme[kGLThemeKeyRowHighlightColor]     = "#1aeeeeec"; // text
		theme[kGLThemeWordCountBackgroundColor] = "#99eeeeec"; // text
		theme[kGLThemeWordCountTextColor]       = "#2e3436";   // background
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_tango.png";
		theme[kGLThemeKeyType]                  = "dark";
		mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
		mThemeNameList << theme[kGLThemeKeyName].toString();
	}

	return;
//  Theme Initialization
{
	QVariantMap theme;
	theme[kGLThemeKeyName]                  = "";
	theme[kGLThemeKeyTextColor]             = "#";//
	theme[kGLThemeKeyBackgroundColor]       = "#";//
	theme[kGLThemeKeyDividerColor]          = "#55"; // text
	theme[kGLThemeKeyTitleDividerColor]     = "#99"; // text
	theme[kGLThemeKeyRowHighlightColor]     = "#1a"; // text
	theme[kGLThemeWordCountBackgroundColor] = "#99"; // text
	theme[kGLThemeWordCountTextColor]       = "#";   // background
	theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_.png";
	theme[kGLThemeKeyType]                  = "dark";
	mThemeDictionary.insert( theme[kGLThemeKeyName].toString(), theme );
	mThemeNameList << theme[kGLThemeKeyName].toString();
}
}

QVariantList ThemeManager::themeList() {
	QVariantList result;

	for (int i = 0; i < mThemeNameList.size();i++) {
		QString themeName = mThemeNameList.value(i);
		QVariantMap theme( mThemeDictionary[themeName].toMap() );
		theme[kGLThemeKeyActive] = (theme[kGLThemeKeyName].toString() == mCurrentThemeName );
		result << theme;
	}

	return result;
}

QVariantMap ThemeManager::currentTheme() {
	QVariantMap theme = mThemeDictionary.value( mCurrentThemeName ).toMap();
	theme[kGLThemeKeyActive] = true;
	return theme;
}

QString ThemeManager::currentThemeName() {
	return mCurrentThemeName;
}

QString ThemeManager::defaultThemeName() {
	return "Light";
}

void ThemeManager::setTheme(QString themeName) {
	// themeName must exists
	if (!mThemeDictionary.keys().contains(themeName)) return;

	if (mCurrentThemeName != themeName) {
		mCurrentThemeName = themeName;
		saveThemeToSettings( themeName );
		emit themeChanged( currentTheme() );
	}
}

} /* namespace writely */
