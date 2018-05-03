#include <iostream>
#include <irrlicht.h>
#include <irrKlang.h>
#include "chessboard.h"
#include <QCoreApplication>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace irrklang;
using namespace std;

ISceneManager* smgr; /**< TODO: describe */
IGUIEnvironment* guienv; /**< TODO: describe */
IAnimatedMeshSceneNode* node; /**< TODO: describe */
IrrlichtDevice *device; /**< TODO: describe */
IVideoDriver* driver; /**< TODO: describe */

ISoundEngine* engine; /**< TODO: describe */
ISoundSource *soundInsert; /**< TODO: describe */
ISoundSource *soundClick; /**< TODO: describe */
ISoundSource *soundBtn; /**< TODO: describe */

/**
 * @brief
 *
 */
enum
{
    GUI_BUTTON_INSERT = 100 ,
    GUI_BUTTON_TAKEBACK,
    GUI_BUTTON_RESET ,
    GUI_BUTTON_ABOUT ,
    GUI_BUTTON_QUIT
};

int chessBoardZLevel[4][4]; /**< TODO: describe */
IGUIButton *btnInsert; /**< TODO: describe */
IGUIButton *btnTakeback; /**< TODO: describe */
IGUIButton *btnReset; /**< TODO: describe */
int currentChosenPos; /**< TODO: describe */
bool isGameOver = false; /**< TODO: describe */

// We'll create a struct to record info on the mouse state
/**
 * @brief Mouse status.
 *
 */
struct SMouseState
{
    core::position2di Position; /**< TODO: describe */
    bool LeftButtonDown; /**< TODO: describe */
    /**
     * @brief
     *
     */
    SMouseState() : LeftButtonDown(false) { }
} MouseState; /**< TODO: describe */

ISceneNode *highlightNode = 0, *lastHighlightNode = 0, *lastWhiteNode = 0; /**< TODO: describe */
scene::ISceneCollisionManager *collmgr = 0; /**< TODO: describe */
IGUIStaticText *winBillboard = 0; /**< TODO: describe */
ChessBoard cb; /**< TODO: describe */

IAnimatedMesh* tempPices; /**< TODO: describe */
IAnimatedMeshSceneNode* tempPicesNode; /**< TODO: describe */

vector<IAnimatedMeshSceneNode*> picesGroup; /**< TODO: describe */

/**
 * @brief Return which stich has chosend.
 *
 * @param num
 * @return core::position2di
 */
core::position2di getCoordinateFromNum(int num)
{
    core::position2di ans;

    switch (num) {
    case 4:
        ans.X = 3;
        ans.Y = 0;
        break;
    case 8:
        ans.X = 3;
        ans.Y = 1;
        break;
    case 12:
        ans.X = 3;
        ans.Y = 2;
        break;
    case 16:
        ans.X = 3;
        ans.Y = 3;
        break;
    default:
        ans.X = num % 4 - 1;
        ans.Y = num / 4;
        break;
    }

    return ans;
}

/**
 * @brief Main UI handler
 *
 */
class MyEventReceiver : public IEventReceiver
{
public:
    // This is the one method that we have to implement
    /**
     * @brief On event.
     *
     * @param event
     * @return bool
     */
    virtual bool OnEvent(const SEvent& event)
    {
        // Remember the mouse state
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
            core::position2di insertPos;

            switch(event.MouseInput.Event)
            {
            case EMIE_LMOUSE_PRESSED_DOWN:

                if(isGameOver)
                    break;

                MouseState.LeftButtonDown = true;
                collmgr = smgr->getSceneCollisionManager();

                if(collmgr->getSceneNodeFromScreenCoordinatesBB(MouseState.Position,-1,false,0)->getID() > 0)
                {

                    int tempIndex = collmgr->getSceneNodeFromScreenCoordinatesBB(
                                MouseState.Position,-1,false,0)->getID();
                    position2di temp = getCoordinateFromNum(tempIndex);

                    if(chessBoardZLevel[temp.X][temp.Y] > 3)
                        break;

                    highlightNode = collmgr->getSceneNodeFromScreenCoordinatesBB(MouseState.Position,-1,false,0);
                    stickChosenIndex = highlightNode->getID();
                    insertPos = getCoordinateFromNum(stickChosenIndex);

                    engine->play2D(soundInsert);

                    if(lastHighlightNode)
                        lastHighlightNode->setMaterialFlag(EMF_LIGHTING,true);

                    if(highlightNode == lastHighlightNode)
                    {
                        if(tempPicesNode)
                        {
                            highlightNode->setMaterialFlag(EMF_LIGHTING,true);
                            btnInsert->setEnabled(false);
                            tempPicesNode->remove();
                            tempPicesNode = 0;
                            stickChosenIndex = -1;
                            break;
                        }
                    }

                    if(tempPicesNode)
                    {

                        tempPicesNode->remove();
                        tempPicesNode = 0;

                    }

                    lastHighlightNode = highlightNode;

                    highlightNode->setMaterialFlag(EMF_LIGHTING,false);

                    stickChosenIndex = highlightNode->getID();
                    insertPos = getCoordinateFromNum(stickChosenIndex);

                    if(chessBoardZLevel[insertPos.X][insertPos.Y] < 4)
                    {
                        tempPices = smgr->getMesh("../media/graphy/black.obj");
                        tempPicesNode = smgr->addAnimatedMeshSceneNode(tempPices);
                        if(tempPicesNode)
                        {
                            tempPicesNode->setPosition(core::vector3df(insertPos.Y * 10,
                                                                 (chessBoardZLevel[insertPos.X][insertPos.Y]) * 4.7,
                                                                 insertPos.X * 10));
                            tempPicesNode->setScale(vector3df(3,3,3));
                            tempPicesNode->setMaterialFlag(EMF_LIGHTING, true);
                            tempPicesNode->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
                            tempPicesNode->setMD2Animation(scene::EMAT_STAND);
                            tempPicesNode->setMaterialTexture( 0,
                                              driver->getTexture("../media/graphy/black.tga") );
                            tempPicesNode->addShadowVolumeSceneNode();
                            tempPicesNode->setID(0);
                        }
                        btnInsert->setEnabled(true);
                    }


                }

                break;

            case EMIE_LMOUSE_LEFT_UP:

                if(isGameOver)
                    break;

                if(MouseState.LeftButtonDown == true &&
                        collmgr->getSceneNodeFromScreenCoordinatesBB(MouseState.Position,-1,false,0)->getID() > 0)
                {

                    MouseState.LeftButtonDown = false;
                    //cout<<MouseState.Position.X<<" "<<MouseState.Position.Y<<" "<<stickChosenIndex<<endl;
                    if(stickChosenIndex != -1)
                        btnInsert->setEnabled(true);

                }

                break;

            case EMIE_MOUSE_MOVED:
                MouseState.Position.X = event.MouseInput.X;
                MouseState.Position.Y = event.MouseInput.Y;

                break;

            default:
                // We won't use the wheel
                break;
            }
        }

        if(event.EventType == irr::EET_GUI_EVENT)
        {
            IAnimatedMesh* blackPices, *whitePices;
            IAnimatedMeshSceneNode* blackPicesNode, *whitePicesNode;
            core::position2di insertPos;
            IAnimatedMeshSceneNode *node;
            PicesPosList list;

            int status;
            s32 id = event.GUIEvent.Caller->getID();
            IGUIWindow* window;
            if(event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
            {
                switch (id) {
                case GUI_BUTTON_INSERT:
                    //cout<<"INSERT"<<endl;

                    engine->play2D(soundClick);

                    insertPos = getCoordinateFromNum(stickChosenIndex);
                    if(chessBoardZLevel[insertPos.X][insertPos.Y] > 3)
                        break;

                    highlightNode->setMaterialFlag(EMF_LIGHTING,true);
                    tempPicesNode->remove();
                    tempPicesNode = 0;

                    blackPices = smgr->getMesh("../media/graphy/black.obj");
                    blackPicesNode = smgr->addAnimatedMeshSceneNode(blackPices);
                    picesGroup.push_back(blackPicesNode);
                    if(blackPicesNode)
                    {
                        blackPicesNode->setPosition(core::vector3df(insertPos.Y * 10,
                                                             chessBoardZLevel[insertPos.X][insertPos.Y] * 4.7,
                                                             insertPos.X * 10));
                        blackPicesNode->setScale(vector3df(3,3,3));
                        blackPicesNode->setMaterialFlag(EMF_LIGHTING, true);
                        blackPicesNode->setMD2Animation(scene::EMAT_STAND);
                        blackPicesNode->setMaterialTexture( 0,
                                          driver->getTexture("../media/graphy/black.tga") );
                        blackPicesNode->addShadowVolumeSceneNode();
                        blackPicesNode->setID(0);
                        chessBoardZLevel[insertPos.X][insertPos.Y]++;
                    }

                    if(!cb.insertPices(insertPos.X, insertPos.Y, chessPicesStatus::black))
                    {
                        blackPicesNode->remove();
                        highlightNode = NULL;
                        break;
                    }

                    _sleep(150);
                    status = cb.isWin(cb.chessBoard);
                    switch (status) {
                    case chessPicesStatus::white:
                        winBillboard = guienv->addStaticText(L"You lose!", rect<s32>(10,10,500,100),true);
                        isGameOver = true;
                        highlightNode = NULL;
                        btnInsert->setEnabled(false);
                        engine->play2D("../media/sound/gameover.mp3");
                        break;
                    case chessPicesStatus::black:
                        winBillboard = guienv->addStaticText(L"You win!", rect<s32>(10,10,500,100),true);
                        isGameOver = true;
                        highlightNode = NULL;
                        btnInsert->setEnabled(false);
                        engine->play2D("../media/sound/win.wav");
                        break;
                    default:
                        highlightNode = NULL;
                        btnInsert->setEnabled(false);
                        break;
                    }

                    if(isGameOver)
                        break;

                    cb.dfs(cb.chessBoard,2);
                    cb.insertPices(cb.targetPos.x, cb.targetPos.y, chessPicesStatus::white);

                    whitePices = smgr->getMesh("../media/graphy/white.obj");
                    whitePicesNode = smgr->addAnimatedMeshSceneNode(whitePices);
                    picesGroup.push_back(whitePicesNode);

                    if(whitePicesNode)
                    {
                        if(lastWhiteNode)
                            lastWhiteNode->setMaterialFlag(EMF_LIGHTING, true);

                        lastWhiteNode = whitePicesNode;

                        whitePicesNode->setPosition(core::vector3df(cb.targetPos.y * 10,
                                                                    cb.targetPos.z * 4.7,
                                                                    cb.targetPos.x * 10));
                        whitePicesNode->setScale(vector3df(3,3,3));
                        whitePicesNode->setMaterialFlag(EMF_LIGHTING, false);
                        whitePicesNode->setMD2Animation(scene::EMAT_STAND);
                        whitePicesNode->setMaterialTexture( 0,
                                          driver->getTexture("../media/graphy/white.tga") );
                        whitePicesNode->addShadowVolumeSceneNode();
                        whitePicesNode->setID(0);
                        chessBoardZLevel[cb.targetPos.x][cb.targetPos.y]++;
                        //cout<<endl<<"*******Insert x:"<<cb.targetPos.x<<" y:"<<cb.targetPos.y<<" z:"
                           //<<cb.targetPos.z<<"********"<<endl<<endl;
                    }

                    guienv = device->getGUIEnvironment();

                    status = cb.isWin(cb.chessBoard);
                    switch (status) {
                    case chessPicesStatus::white:
                        winBillboard = guienv->addStaticText(L"You lose!", rect<s32>(10,10,500,100),true);
                        isGameOver = true;
                        highlightNode = NULL;
                        btnInsert->setEnabled(false);
                        engine->play2D("../media/sound/gameover.mp3");
                        break;
                    case chessPicesStatus::black:
                        winBillboard = guienv->addStaticText(L"You win!", rect<s32>(10,10,500,100),true);
                        isGameOver = true;
                        highlightNode = NULL;
                        btnInsert->setEnabled(false);
                        engine->play2D("../media/sound/win.wav");
                        break;
                    default:
                        highlightNode = NULL;
                        btnInsert->setEnabled(false);
                        break;
                    }
                    break;
                case GUI_BUTTON_TAKEBACK:
                    engine->play2D(soundBtn);

                    if(lastWhiteNode)
                        lastHighlightNode->setMaterialFlag(EMF_LIGHTING, true);
                    lastHighlightNode = 0;
                    highlightNode = 0;

                    int takebackNum;
                    if(cb.isWin(cb.chessBoard) == chessPicesStatus::black)
                        takebackNum = 1;
                    else
                        takebackNum = 2;

                    list = cb.takeback(cb.chessBoard, takebackNum);
                    if(list.size() == 0)
                        break;
                    for(auto iter = list.begin();iter != list.end();iter++)
                    {
                        chessBoardZLevel[iter->x][iter->y]--;
                    }

                    for(int i=0;i<takebackNum;i++)
                    {
                        node = picesGroup.back();
                        node->remove();
                        picesGroup.pop_back();
                    }
                    if(isGameOver)
                    {
                        isGameOver = false;
                        winBillboard->remove();
                        winBillboard = 0;
                    }
                    if(tempPicesNode)
                    {
                        tempPicesNode->remove();
                        tempPicesNode = 0;
                    }
                    btnInsert->setEnabled(false);
                    lastWhiteNode = 0;
                    break;
                case GUI_BUTTON_RESET:
                    engine->play2D(soundBtn);

                    cb.init();
                    for(auto iter = picesGroup.begin();iter != picesGroup.end();iter++)
                    {
                        IAnimatedMeshSceneNode *node = *iter;
                        node->remove();
                    }
                    if(lastHighlightNode)
                        lastHighlightNode->setMaterialFlag(EMF_LIGHTING,true);
                    isGameOver = false;
                    for(int i=0;i<4;i++)
                        for(int j=0;j<4;j++)
                            chessBoardZLevel[i][j]=0;
                    if(winBillboard)
                    {
                        winBillboard->remove();
                        winBillboard = 0;
                        //guienv->clear();
                    }
                    picesGroup.clear();
                    btnInsert->setEnabled(false);
                    if(tempPicesNode)
                    {
                        tempPicesNode->remove();
                        tempPicesNode = 0;
                    }
                    lastWhiteNode = 0;
                    break;
                case GUI_BUTTON_ABOUT:

                    window = guienv->addWindow(
                        rect<s32>(100 , 100 , 380, 240),
                        false, // modal?
                        L"About");

                    guienv->addStaticText(L"Author: Scobbing",
                        rect<s32>(35,35,35+120,35+30),
                        false, // border?
                        false, // wordwrap?
                        window);

                    guienv->addStaticText(L"scobbing_ad@163.com",
                        rect<s32>(35,55,35+170,55+30),
                        false, // border?
                        false, // wordwrap?
                        window);

                    guienv->addStaticText(L"This is an opensource software",
                        rect<s32>(35,75,35+210,75+30),
                        false, // border?
                        false, // wordwrap?
                        window);

                    guienv->addStaticText(L"following MIT license.",
                        rect<s32>(35,95,35+210,95+30),
                        false, // border?
                        false, // wordwrap?
                        window);

                    break;
                case GUI_BUTTON_QUIT:
                    engine->play2D(soundBtn);
                    //cout<<"QUIT"<<endl;
                    device->closeDevice();
                    return true;
                default:
                    break;
                }
            }
        }

        return false;
    }

    /**
     * @brief
     *
     * @return const SMouseState
     */
    const SMouseState & GetMouseState(void) const
    {
        return MouseState;
    }


    /**
     * @brief
     *
     */
    MyEventReceiver()
    {
    }

private:
    int stickChosenIndex = 0; /**< TODO: describe */
};

/**
 * @brief Enter point.
 *
 * @param argc
 * @param argv[]
 * @return int
 */
int main(int argc, char *argv[])
{
    cb.init();
    engine = createIrrKlangDevice();
    soundInsert = engine->addSoundSourceFromFile(
                "../media/sound/select.wav", ESM_AUTO_DETECT, true);
    soundClick = engine->addSoundSourceFromFile(
                "../media/sound/insert.mp3", ESM_AUTO_DETECT, true);
    soundBtn = engine->addSoundSourceFromFile(
                "../media/sound/button.mp3", ESM_AUTO_DETECT, true);

    MyEventReceiver receiver;

    SIrrlichtCreationParameters params = SIrrlichtCreationParameters();
    params.AntiAlias = 8;
    params.DriverType = video::EDT_OPENGL;
    params.WindowSize = core::dimension2d<u32>(1920, 1080);
    params.EventReceiver = &receiver;
    params.Vsync = true;
    params.Bits = 32;
    params.DriverMultithreaded = true;
    params.Stencilbuffer = true;

    device = createDeviceEx(params);

    if (!device)
        return 1;

    /*
    Set the caption of the window to some nice text. Note that there is an
    'L' in front of the string. The Irrlicht Engine uses wide character
    strings when displaying text.
    */
    device->setWindowCaption(L"4D Chess Playground");

    /*
    Get a pointer to the VideoDriver, the SceneManager and the graphical
    user interface environment, so that we do not always have to write
    device->getVideoDriver(), device->getSceneManager(), or
    device->getGUIEnvironment().
    */
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();

    gui::IGUIFont* fnt = guienv->getFont("../media/font/myfont.xml");
    guienv->getSkin()->setFont(fnt);

    driver->setAmbientLight(SColorf(1.f,1.f,1.f,1.f));

    btnInsert = guienv->addButton(rect<s32>(10,240,110,240 + 32), 0, GUI_BUTTON_INSERT,
            L"Insert", L"Insert Pices");
    btnTakeback = guienv->addButton(rect<s32>(10,300,110,300 + 32), 0, GUI_BUTTON_TAKEBACK,
            L"Take Back", L"Take Back Pices");
    btnReset = guienv->addButton(rect<s32>(10,360,110,360 + 32), 0, GUI_BUTTON_RESET,
            L"Reset", L"Reset Chess Board");
    guienv->addButton(rect<s32>(10,420,110,420 + 32), 0, GUI_BUTTON_ABOUT,
            L"About", L"About this Program");
    guienv->addButton(rect<s32>(10,480,110,480 + 32), 0, GUI_BUTTON_QUIT,
            L"Quit", L"Exits Program");

    btnInsert->setEnabled(false);
    btnTakeback->setEnabled(false);

    IAnimatedMesh* mesh = smgr->getMesh("../media/graphy/cb2.obj");

    if (!mesh)
    {
        device->drop();
        return 1;
    }
    node = smgr->addAnimatedMeshSceneNode( mesh );

    if (node)
    {
        node->setScale(vector3df(3,3,3));
        node->setMaterialFlag(EMF_LIGHTING, true);
        node->setMD2Animation(scene::EMAT_STAND);
        node->setMaterialTexture( 0, driver->getTexture("../media/graphy/light.tga") );
        node->addShadowVolumeSceneNode();
        node->setID(0);
        smgr->setShadowColor(video::SColor(150, 0, 0, 0));//全局的
    }

    IAnimatedMeshSceneNode* stickNode[16];
    int n = 0;

    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            IAnimatedMesh* stick = smgr->getMesh("../media/graphy/stick001.obj");
            stickNode[n] = smgr->addAnimatedMeshSceneNode(stick);
            if(stickNode[n])
            {
                stickNode[n]->setScale(vector3df(3,3,3));
                stickNode[n]->setPosition(vector3df(i * 10, 0 , j * 10));
                stickNode[n]->setMD2Animation(scene::EMAT_STAND);
                stickNode[n]->setMaterialFlag(video::EMF_LIGHTING,true);
                stickNode[n]->setMaterialTexture( 0,
                              driver->getTexture("../media/graphy/stick.tga") );
                stickNode[n]->addShadowVolumeSceneNode();
                stickNode[n]->setID(n + 1);
                n++;
            }
        }
    }

    scene::ISceneNode* SkyBox = 0;
    SkyBox = smgr->addSkyBoxSceneNode(
        driver->getTexture("../media/skybox/shinei_up.jpg"),
        driver->getTexture("../media/skybox/shinei_dn.jpg"),
        driver->getTexture("../media/skybox/shinei_rt.jpg"),
        driver->getTexture("../media/skybox/shinei_lf.jpg"),
        driver->getTexture("../media/skybox/shinei_ft.jpg"),
        driver->getTexture("../media/skybox/shinei_bk.jpg"));

    scene::ILightSceneNode* light1 = smgr->addLightSceneNode(0, core::vector3df(-30,90,5),
        video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 1000.0f);
    scene::ILightSceneNode* light2 = smgr->addLightSceneNode(0, core::vector3df(30,90,10),
        video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 1000.0f);
    //scene::ILightSceneNode* light3 = smgr->addLightSceneNode(0, core::vector3df(0,100,0),
    //   video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 2000.0f);


    ICameraSceneNode *camera = smgr->addCameraSceneNodeMaya();
    camera->setFarValue(8000.f);
    camera->setFOV(PI/3.0f);
    camera->setTarget(core::vector3df(0,0,0));
    smgr->setActiveCamera(camera);

    while(device->run())
    {

        driver->beginScene(true, true, SColor(255,100,101,140));
        smgr->drawAll();
        guienv->drawAll();

        if(picesGroup.size() >= 2)
        {
            btnTakeback->setEnabled(true);
            btnReset->setEnabled(true);
        }
        else
        {
            btnReset->setEnabled(false);
            btnTakeback->setEnabled(false);
        }

        driver->endScene();
    }

    device->drop();
    return 0;
}
