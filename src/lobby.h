#pragma once

#include "assetpathresolver.h"
#include "file_functions.h"
#include "network/masterapigateway.h"
#include "networkmanager.h"

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTextBrowser>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#ifdef ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

class AOApplication;

class Lobby : public QMainWindow
{
  Q_OBJECT

public:
  enum ServerType
  {
    Public,
    Favorite,
  };

  explicit Lobby(Options &options, AOApplication &application, kal::AssetPathResolver &assetPathResolver, kal::NetworkManager &network, QWidget *parent = nullptr);
  ~Lobby();

  ServerType currentServerType() const;
  std::optional<kal::ServerInfo> currentServer() const;

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  enum SortMode
  {
    NoSort,
    Ascending,
    Descending,
  };

  Options &options;
  AOApplication &ao_app;
  kal::AssetPathResolver &m_resolver;
  kal::NetworkManager &m_network;
  kal::MasterApiGateway m_master;

  ServerType m_tab = Public;
  SortMode m_sort = NoSort;
  QList<kal::ServerInfo> m_public_list;
  QList<kal::ServerInfo> m_favorite_list;

  QLabel *ui_version;
  QPushButton *ui_options;
  QPushButton *ui_about;

  QTabWidget *ui_tabs;

  QTreeWidget *ui_public_tree;
  QLineEdit *ui_public_search;
  QPushButton *ui_direct_connect;
  QPushButton *ui_add_to_favorite;

  QTreeWidget *ui_favorite_tree;
  QPushButton *ui_add_favorite;
  QPushButton *ui_remove_favorite;
  QPushButton *ui_edit_favorite;

  QPushButton *ui_refresh;

  QTextBrowser *ui_description;
  QPushButton *ui_connect;

  QTextBrowser *ui_motd;

  void setupInterface();

  void updateFavoriteList();
  void updateServerList(QTreeWidget *tree, const QList<kal::ServerInfo> &serverList);
  void sortServerList();
  void sortServerList(QTreeWidget *tree);

  void displayCurrentServerDescription();

private Q_SLOTS:
  void openAbout();
  void openOptions();
  void openDirectConnect();

  void displayMasterError(const QString &error);
  void checkMasterVersion();

  void updateMessageOfTheDay();

  void resetCurrentServerSelection();
  void refreshServerList();

  void updatePublicList();
  void filterPublicList();

  void addToFavorite();
  void addFavorite();
  void editFavorite();
  void removeFavorite();

  void nextServerSortMode();

  void setupCurrentServer();
  void connectToCurrentServer();
};
