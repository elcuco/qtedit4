#pragma once

#include <QAbstractItemModel>
#include <QCompleter>
#include <QDir>
#include <QFileSystemModel>
#include <QList>
#include <QSortFilterProxyModel>
#include <QString>

bool FilenameMatches(const QString &fileName, const QString &goodList, const QString &badList);

class DirectoryModel : public QAbstractTableModel {
  public:
    DirectoryModel(QObject *parent = nullptr);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    const QString &getItem(size_t i) const { return fileList[i]; }
    QString displayForItem(size_t i) const;
    QString fileNameForItem(size_t i) const;

    void removeAllDirs();
    void addDirectory(const QString &path);
    void removeDirectory(const QString &path);

    QStringList fileList;
    QStringList directoryList;

  private:
    void addDirectoryImpl(const QDir &directory);
    void removeDirectoryImpl(const QDir &directory);
};

class FilterOutProxyModel : public QSortFilterProxyModel {
  public:
    explicit FilterOutProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}

    void setFilterWildcards(const QString &wildcards) {
        m_filterWildcards = wildcards;
        invalidateFilter();
    }

    void setFilterOutWildcard(const QString &wildcard) {
        m_filterOutWildcard = wildcard;
        invalidateFilter();
    }

  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
        QString filePath = sourceModel()->data(index, Qt::DisplayRole).toString();
        return FilenameMatches(filePath, m_filterWildcards, m_filterOutWildcard);
    }

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override {
        QString leftData = sourceModel()->data(left).toString();
        QString rightData = sourceModel()->data(right).toString();

        auto l = countOccurrences(leftData, '/');
        auto r = countOccurrences(rightData, '/');
        if (l < r) {
            return true;
        }
        if (l > r) {
            return false;
        }
        return QSortFilterProxyModel::lessThan(left, right);
    }

    static int countOccurrences(const QString &str, QChar target) {
        int count = 0;
        for (const QChar &ch : str) {
            if (ch == target) {
                count++;
            }
        }
        return count;
    }

  private:
    QString m_filterWildcards;
    QString m_filterOutWildcard;
};
