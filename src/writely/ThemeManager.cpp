/*
 * ThemeManager.cpp
 *
 *  Created on: Aug 3, 2013
 *      Author: iMac
 */

#include "ThemeManager.h"
#include "GLConstants.h"

namespace writely {

ThemeManager::ThemeManager() {
	mCurrentThemeName = defaultThemeName();
}

ThemeManager::~ThemeManager() {
}

QVariantMap ThemeManager::currentTheme() {
	if (mCurrentThemeName == defaultThemeName()) {
		QVariantMap theme;
		theme[kGLThemeKeyName]         = mCurrentThemeName;
		theme[kGLThemeKeyDividerColor] = "#e4e4e4";
		return theme;
	}

	QVariantMap theme;
	if (mCurrentThemeName == "Dark") {
		theme[kGLThemeKeyName]                  = "Dark";
		theme[kGLThemeKeyDividerColor]          = "#404040";
		theme[kGLThemeKeyTitleDividerColor]     = "#7f7f7f";
		theme[kGLThemeKeyBackgroundColor]       = "#000000";
		theme[kGLThemeKeyTextColor]             = "#ffffff";
		theme[kGLThemeKeyRowHighlightColor]     = "#a1a1a1";
		theme[kGLThemeWordCountBackgroundColor] = "#99ffffff";
		theme[kGLThemeWordCountTextColor]       = "#000000";
		theme[kGLThemeWordCountDotsImageSource] = "asset:///images/dots_th_dark.png";
	}

	return theme;
}

QString ThemeManager::currentThemeName() {
	return mCurrentThemeName;
}

QString ThemeManager::defaultThemeName() {
	return "Light";
}

void ThemeManager::setTheme(QString themeName) {
	if (mCurrentThemeName != themeName) {
		mCurrentThemeName = themeName;
		emit themeChanged( currentTheme() );
	}
}

} /* namespace writely */
