/* 
---------------------------------------------------------------- 
---------------------------------------------------------------- 
*/ 
 
#define OIS_DYNAMIC_LIB 
#include "Ogre.h" 
#include "OgreFrameListener.h" 
#include <OIS/OIS.h>

#define UNUSEDVAR(...) (__VA_ARGS__) 
using namespace Ogre; 
 
////////////////////////////////////////////////////////////////////////// 
// class SimpleFrameListener 
////////////////////////////////////////////////////////////////////////// 
class SimpleFrameListener : public FrameListener 
{ 
public: 
    SimpleFrameListener(OIS::Keyboard* keyboard, OIS::Mouse* mouse) 
    { 
        mKeyboard = keyboard; 
        mMouse = mouse; 
    } 
    // This gets called before the next frame is beeing rendered.
    bool frameStarted(const FrameEvent& evt) 
    {
        //update the input devices
        mKeyboard->capture();
        mMouse->capture();
 
        //exit if key KC_ESCAPE pressed 
        if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) 
            return false; 
 
        return true; 
    } 
    // This gets called at the end of a frame.
    bool frameEnded(const FrameEvent& evt) 
    { 
        return true; 
    } 
private: 
    OIS::Keyboard* mKeyboard; 
    OIS::Mouse* mMouse; 
}; 
 
////////////////////////////////////////////////////////////////////////// 
// class SimpleKeyListener 
////////////////////////////////////////////////////////////////////////// 
class SimpleKeyListener : public OIS::KeyListener 
{ 
public: 
    bool keyPressed(const OIS::KeyEvent& e){ return true; }
 
    bool keyReleased(const OIS::KeyEvent& e){ return true; }
};
 
////////////////////////////////////////////////////////////////////////// 
// class SimpleMouseListener 
////////////////////////////////////////////////////////////////////////// 
class SimpleMouseListener : public OIS::MouseListener
{
public: 
    bool mouseMoved(const OIS::MouseEvent& e){ return true; }
 
    bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id){ return true; }
 
    bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id){ return true; }
};
 
////////////////////////////////////////////////////////////////////////// 
// main 
////////////////////////////////////////////////////////////////////////// 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    #define WIN32_LEAN_AND_MEAN
    #include "windows.h"
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char **argv)
#endif
{
    // root
    Root* root = new Root;

    // resources / filesystems
    ResourceGroupManager::getSingleton().addResourceLocation("../base/", "FileSystem");
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    //  plugins
#if OGRE_DEBUG_MODE == 1
    root->loadPlugin("RenderSystem_Direct3D11_d.dll");
    root->loadPlugin("Plugin_OctreeSceneManager_d.dll");
#else
    root->loadPlugin("RenderSystem_Direct3D11.dll");
    root->loadPlugin("Plugin_OctreeSceneManager.dll");
#endif

    // Configures the application
    if(!root->restoreConfig() && !root->showConfigDialog())
    {
        delete root;
        return false;
    } 

    // creates the window
    RenderWindow* window = root->initialise(true, "Lola Game");
    if (window->isFullScreen())
        window->setFullscreen(false, window->getWidth(), window->getHeight());
 
    // scene manager
    SceneManager* sceneMgr = root->createSceneManager(ST_GENERIC); 
 
    // 5 Create the camera 
    Camera* camera = sceneMgr->createCamera("SimpleCamera"); 
 
    // 6 Create one viewport, entire window 
    Viewport* viewPort = window->addViewport(camera);
    UNUSEDVAR(viewPort);

    // 7 add OIS input handling 
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
 
    //tell OIS about the Ogre window
    window->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
 
    //setup the manager, keyboard and mouse to handle input
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(pl);
    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
    OIS::Mouse*    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
 
    //tell OIS about the window's dimensions
    unsigned int width, height, depth;
    int top, left;
    window->getMetrics(width, height, depth, left, top);
    const OIS::MouseState &ms = mouse->getMouseState();
    ms.width = width;
    ms.height = height;
 
    // everything is set up, now we listen for input and frames (replaces while loops)
    //key events
    SimpleKeyListener* keyListener = new SimpleKeyListener();
    keyboard->setEventCallback(keyListener);
    //mouse events
    SimpleMouseListener* mouseListener = new SimpleMouseListener();
    mouse->setEventCallback(mouseListener);
    //render events
    SimpleFrameListener* frameListener = new SimpleFrameListener(keyboard, mouse);
    root->addFrameListener(frameListener); 
 
    //----------------------------------------------------
    // 8 start rendering 
    //----------------------------------------------------
    root->startRendering(); // blocks until a frame listener returns false. eg from pressing escape in this example
 
    //----------------------------------------------------
    // 9 clean 
    //----------------------------------------------------
    //OIS
    inputManager->destroyInputObject(mouse); mouse = 0;
    inputManager->destroyInputObject(keyboard); keyboard = 0;
    OIS::InputManager::destroyInputSystem(inputManager); inputManager = 0;
    //listeners
    delete frameListener; 
    delete mouseListener; 
    delete keyListener;
    //Ogre
    delete root;
 
    return 0; 
}