/**
 * \file texteditor_plg
 * \brief Definition of
 * \author Diego Iastrubni diegoiast@gmail.com
 * License GPL 2008
 * \see class name
 */

#pragma once

#include "endlinestyle.h"
#include "iplugin.h"

class TextEditorPlugin : public IPlugin {
    struct Config {
        CONFIG_DEFINE(TrimSpaces, bool)
        CONFIG_DEFINE(SmartHome, bool)
        CONFIG_DEFINE(WrapLines, bool);
        CONFIG_DEFINE(AutoReload, bool)
        CONFIG_DEFINE(ShowWhite, bool)
        CONFIG_DEFINE(ShowIndentations, bool)
        CONFIG_DEFINE(HighlightBrackets, bool)
        CONFIG_DEFINE(Margin, bool)
        CONFIG_DEFINE(ShowLine, bool)
        CONFIG_DEFINE(MarginOffset, int)
        CONFIG_DEFINE(LineEndingSave, EndLineStyle)
        qmdiPluginConfig *config;
    };
    Config &getConfig() {
        static Config configObject{&this->config};
        return configObject;
    }

    Q_OBJECT
  public:
    TextEditorPlugin();
    ~TextEditorPlugin();

    void showAbout() override;
    QStringList myExtensions() override;
    int canOpenFile(const QString fileName) override;
    bool openFile(const QString fileName, int x = -1, int y = -1, int z = -1) override;
    void navigateFile(qmdiClient *client, int x, int y, int z) override;
    void applySettings(qmdiClient *);

  public slots:
    virtual void configurationHasBeenModified() override;
    void fileNew();
};
