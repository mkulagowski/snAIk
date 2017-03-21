#include <Serialize\BulletWorldImporter\btBulletWorldImporter.h>
#include <string>

void loadFile(const std::string& path)
{
// in your source code, add
    btBulletWorldImporter* fileLoader = new btBulletWorldImporter(m_dynamicsWorld);

    // optionally enable the verbose mode to provide debugging information during file loading (a lot of data is generated, so this option is very slow)
    // fileLoader->setVerboseMode(true);

    // load the contents from the file
    fileLoader->loadFile("testFile.bullet");

}