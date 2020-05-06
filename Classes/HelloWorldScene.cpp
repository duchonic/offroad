/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    if (!initWithPhysics())
    {
        return false;
    }
    // optional: enable debug draw
    //this->getPhysicsWorld()->setDebugDrawMask(0xffff);
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("super off road", "fonts/Marker Felt.ttf", 12);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }
    
    auto _mouseListener = EventListenerMouse::create();
    _mouseListener->onMouseDown = CC_CALLBACK_1(HelloWorld::onMouseDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);
    
    

    
    auto map = TMXTiledMap::create("maps/map1.tmx");
    this->addChild(map, 0, 99);
    
    auto collisionMap = map->getLayer("collision");
    
    _track = map->getObjectGroup("track");
    
    ValueVector objects = _track->getObjects();

    _movingTrack.at(0) = (collisionMap->getPositionAt(Vec2(11,23)));
    _movingTrack.at(1) = (collisionMap->getPositionAt(Vec2(56,24)));
    _movingTrack.at(2) = (collisionMap->getPositionAt(Vec2(66,7)));
    _movingTrack.at(3) = (collisionMap->getPositionAt(Vec2(21,7)));
    
    if(collisionMap != nullptr){
        auto testPosition = collisionMap->getPositionAt(Vec2(3, 4));
        log("position at 3,4 %f, %f", testPosition.x, testPosition.y);
        
        
        for(int y=0; y < 100; y++){
            for(int x=0; x < 100; x++){
                
                if(collisionMap->getTileGIDAt(Vec2(x, y)) != 0){
                    
                    auto collisionBloc = Sprite::create();
                    auto physicsBody = PhysicsBody::createBox(Size(8.0f, 8.0f), PhysicsMaterial(0.1f, 1.0f, 0.0f));
                    physicsBody->setDynamic(false);
                    physicsBody->setContactTestBitmask(0xFFFFFFFF);
                    auto pos = collisionMap->getPositionAt(Vec2(x,y));
                    pos.y += 4;
                    pos.x += 4;
                    collisionBloc->setPosition( pos );
                    collisionBloc->addComponent(physicsBody);
                    this->addChild(collisionBloc, 0);
                }
                
            }
        }
    }
    else{
        log("no layer found with collision");
    }
    
    /* friendly truck */
    
    auto physicsBody = PhysicsBody::createBox(Size(5.0f , 5.0f ), PhysicsMaterial(0.1f, 1.0f, 1.0f));
    physicsBody->setGravityEnable(false);
    physicsBody->setDynamic(true);

    _speed = 10.0f;
    _direction = Vec2(1.0f, 0.0f);
    Vec2 velocity = _speed * _direction;
    
    log("%f,%f", velocity.x, velocity.y  );
    
    _truckSprite = Sprite::create("trucks/truck.png");
    physicsBody->setVelocity( velocity );
    
    _truckSprite->setPosition(collisionMap->getPositionAt(Vec2(50,30)));
    _truckSprite->addComponent(physicsBody);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    this->addChild(_truckSprite, 3);
    
    
    /* enemy truck */
    
    physicsBody = PhysicsBody::createBox(Size(5.0f , 5.0f ), PhysicsMaterial(0.1f, 1.0f, 1.0f));
    physicsBody->setGravityEnable(false);
    physicsBody->setDynamic(true);

    
    
    _speed2 = 50.0f;
    _direction2 = Vec2(1.0f, 0.0f);
    velocity = _speed2 * _direction2;
    
    log("%f,%f", velocity.x, velocity.y  );
    
    _enemySprite = Sprite::create("trucks/truck.png");
    physicsBody->setVelocity( velocity );
    _enemySprite->setPosition(collisionMap->getPositionAt(Vec2(7,22)));
    _enemySprite->addComponent(physicsBody);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    this->addChild(_enemySprite, 3);
    
    
    
    //adds contact event listener
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    
    
    
    // creating a keyboard event listener
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    schedule(CC_SCHEDULE_SELECTOR(HelloWorld::tick), 0.1f);
    
    return true;
}


void HelloWorld::onMouseDown(Event *event)
{
    // to illustrate the event....
    EventMouse* e = (EventMouse*)event;
    
    log("mouse was pressed : %u", e->getMouseButton());
    
    auto pos = Vec2(floor( e->getCursorX()/8 ), floor( (800.0f - e->getCursorY())/8 ));
    
    log("position: %f / %f", pos.x, pos.y);
}


// Implementation of the keyboard event callback function prototype
void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode) {
        case cocos2d::EventKeyboard::KeyCode::KEY_1:
            log("key 1 pressed");
            _speed += 10;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_2:{
            log("key 2 pressed");
            if(_speed > 0){
                _speed -= 10;
            }
            break;
        }
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:{
            _direction.rotate(Vec2(0.0f, 0.0f), 3.14/4);
            break;
        }
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:{
            _direction.rotate(Vec2(0.0f, 0.0f), -(3.14/4));
            break;
        }
        default:
            log("Key with keycode %d pressed", keyCode);
            break;
    }
    Vec2 velocity = _speed * _direction;
    _truckSprite->getPhysicsBody()->setVelocity( velocity );
    _truckSprite->setRotation( _direction.getAngle() * (-180/3.14)  );

}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
        log("Key with keycode %d released", keyCode);
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

bool HelloWorld::onContactBegin(PhysicsContact& contact)
{
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    log("collision begin");
    
    if (nodeA && nodeB)
    {
        if (nodeA->getTag() == 10)
        {
            nodeB->removeFromParentAndCleanup(true);
        }
        else if (nodeB->getTag() == 10)
        {
            nodeA->removeFromParentAndCleanup(true);
        }
    }

    //bodies can collide
    return true;
}

void HelloWorld::tick(float dt)
{
    static int enemyState = 0;
    
    Vec2 velocity = _speed * _direction;
    _truckSprite->getPhysicsBody()->setVelocity( velocity );
    _truckSprite->setRotation( _direction.getAngle() * (-180/3.14) + (rand()/(double)RAND_MAX)*10  );
    _truckSprite->getPhysicsBody()->setAngularVelocity(0.0f);
    
    velocity = _speed2 * _direction2.getNormalized();
    _enemySprite->getPhysicsBody()->setVelocity( velocity );
    _enemySprite->setRotation( _direction2.getAngle() * (-180/3.14) + (rand()/(double)RAND_MAX)*10  );
    _enemySprite->getPhysicsBody()->setAngularVelocity(0.0f);
    
    
    if(_direction.x < 0){
        _truckSprite->setScaleY(-1);
    }
    else{
        _truckSprite->setScaleY(1);
    }

    if(_direction2.x < 0){
        _enemySprite->setScaleY(-1);
    }
    else{
        _enemySprite->setScaleY(1);
    }

    
    switch (enemyState) {
        case 0:{
            auto enemy =_enemySprite->getPosition();
            auto target = _movingTrack.at(0);
            
            auto diffx = target.x - enemy.x;
            auto diffy = target.y - enemy.y;
            
            if(abs(diffx + diffy) < 10 ){
                enemyState+=1;
            }
            log("diff x %f y %f", diffx, diffy);
            
            _direction2 = Vec2( diffx, diffy );
            break;
        }
        case 1:{
            auto enemy =_enemySprite->getPosition();
            auto target = _movingTrack.at(1);
            
            auto diffx = target.x - enemy.x;
            auto diffy = target.y - enemy.y;
            log("diff x %f y %f", diffx, diffy);
            if( abs(diffx + diffy) < 10 ){
                enemyState+=1;
            }
            _direction2 = Vec2( diffx, diffy );
            break;
        }
        case 2:{
            auto enemy =_enemySprite->getPosition();
            auto target = _movingTrack.at(2);
            
            auto diffx = target.x - enemy.x;
            auto diffy = target.y - enemy.y;
            log("diff x %f y %f", diffx, diffy);
            if( abs(diffx + diffy) < 10 ){
                enemyState+=1;
            }
            _direction2 = Vec2( diffx, diffy );
            break;
        }
        case 3:{
            auto enemy =_enemySprite->getPosition();
            auto target = _movingTrack.at(3);
            
            auto diffx = target.x - enemy.x;
            auto diffy = target.y - enemy.y;
            log("diff x %f y %f", diffx, diffy);
            if( abs(diffx + diffy) < 10 ){
                enemyState=0;
            }
            _direction2 = Vec2( diffx, diffy );
            break;
        }
        default:
            break;
    }




}
