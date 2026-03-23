#pragma once
#include"DirectX.h"
#include"DirectSound.h"

//所有场景的基类，用于统一接口调用
class Scene
{
public:
    virtual ~Scene() {};
    //场景的初始化函数，在切换场景时会被执行一次
    virtual bool Init() { return true; };
    //场景的关闭函数，在切换场景时会被执行一次
    virtual void End() {};
    //场景的渲染函数
    virtual void Render() {};
    //场景的逻辑更新函数
    virtual void Update() {};
};