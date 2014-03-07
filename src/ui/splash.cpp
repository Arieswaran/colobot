// * This file is part of the COLOBOT source code
// * Copyright (C) 2014 Polish Portal of Colobot (PPC)
// *
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program. If not, see  http://www.gnu.org/licenses/.


#include "ui/splash.h"

#include "common/logger.h"


namespace Ui {

CSplash::CSplash()
{
    m_splashWindow = nullptr;
    m_splashWindowAnim = nullptr;
    UpdateSplashList();
}

CSplash::~CSplash()
{
    if(m_splashWindowAnim != nullptr)
        CEGUI::AnimationManager::getSingleton().destroyAnimationInstance(m_splashWindowAnim);
    
    if(m_splashWindow != nullptr)
        CEGUI::WindowManager::getSingleton().destroyWindow(m_splashWindow);
}

void CSplash::UpdateSplashList()
{
    std::set<CEGUI::String> splashes;
    for(CEGUI::ImageManager::ImageIterator images = CEGUI::ImageManager::getSingleton().getIterator(); !images.isAtEnd(); images++)
    {
        CEGUI::String currentImage = images.getCurrentKey();
        if(currentImage.compare(0, SPLASH_IMAGESET.length()+1, SPLASH_IMAGESET+"/") == 0)
        {
            m_splashes.insert(currentImage);
        }
    }
    
    for(auto s : m_splashes)
        CLogger::GetInstancePointer()->Trace("Found splash image: %s\n", s.c_str());
}

void CSplash::Start()
{
    Next();
}

void CSplash::Next()
{
    if(m_splashWindowAnim != nullptr && m_splashWindowAnim->isRunning())
    {
        CEGUI::AnimationManager::getSingleton().destroyAnimationInstance(m_splashWindowAnim);
        m_splashWindowAnim = nullptr;
    }
    if(m_splashWindow != nullptr)
    {
        CEGUI::WindowManager::getSingleton().destroyWindow(m_splashWindow);
        m_splashWindow = nullptr;
    }
    if(IsFinished()) return;

    CEGUI::String nextImage = *(m_splashes.begin());
    m_splashes.erase(nextImage);
    CLogger::GetInstancePointer()->Debug("Now showing splash: %s\n", nextImage.c_str());

    m_splashWindow = CEGUI::WindowManager::getSingleton().createWindow("Colobot/StaticImage", "splash");
    m_splashWindow->setProperty("Image", nextImage);
    m_splashWindow->moveToBack();
    CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild(m_splashWindow);

    m_splashWindowAnim = CEGUI::AnimationManager::getSingleton().instantiateAnimation(SPLASH_ANIMATION);
    m_splashWindowAnim->setTargetWindow(m_splashWindow);
    m_splashWindow->subscribeEvent(CEGUI::AnimationInstance::EventAnimationEnded, CEGUI::Event::Subscriber(&CSplash::OnAnimationEnded, this));
    m_splashWindowAnim->start();
}

bool CSplash::OnAnimationEnded(const CEGUI::EventArgs& e)
{
    Next();
    return true;
}

bool CSplash::IsFinished()
{
    return (m_splashes.empty() && m_splashWindow == nullptr);
}

}

