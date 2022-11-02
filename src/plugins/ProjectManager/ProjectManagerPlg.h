#pragma once

#include "iplugin.h"
#include <QList>
#include <QAbstractItemModel>
#include <QCompleter>

class QDockWidget;
class QTreeView;
class QCompleter;
class QSortFilterProxyModel;

class FoldersModel;
class DirectoryModel;

namespace Ui{
	class ProjectManagerGUI;
}

class ProjectManagerPlugin : public IPlugin
{
	Q_OBJECT
public:
	ProjectManagerPlugin();

	virtual void	showAbout();
	virtual void	on_client_merged( qmdiHost* host );
	virtual void	on_client_unmerged( qmdiHost* host );
public slots:
	// our code
	void onItemClicked(const QModelIndex &index);
    void on_addDirectory_clicked(bool checked);
    void on_removeDirectory_clicked(bool checked);

private:
    QDockWidget *m_dockWidget;
    QSortFilterProxyModel *filesFilterModel;
    DirectoryModel *directoryModel;
    Ui::ProjectManagerGUI *gui;
};
