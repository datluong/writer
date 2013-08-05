/*
 * GLFolderAcchiver.h
 *
 *  Created on: Aug 5, 2013
 *      Author: iMac
 */

#ifndef GLFOLDERACCHIVER_H_
#define GLFOLDERACCHIVER_H_

#include <QString>
#include <QStringList>

namespace writely {

class GLFolderArchiver {
public:
	GLFolderArchiver();
	virtual ~GLFolderArchiver();

	int zipFolder(const QString &folderName, const QString& saveToPath);
	int countTxtFiles( const QString& archiveFilePath );
	void unarchiveTxtFiles( const QString& archiveFilePath, const QString& extractToFolder);
private:
	void listTxtFiles( const QString& folderName, QStringList* resultList );

};

} /* namespace writely */
#endif /* GLFOLDERACCHIVER_H_ */
