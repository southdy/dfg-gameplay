#include "pch.h"
#include "debug_service.h"
#include "services/service_manager.h"
#include "services/render_service.h"
#include "render/mem_fn_render_click.h"




DebugService::DebugService(const ServiceManager * manager)
    : Service(manager)
    , _systemFont(NULL)
    , _whiteSprite(NULL)
    , _debugRenderStep()
    , _fpsRenderClick()
    , _fontSize(1)
    , _showProfiler(false)
{
}

DebugService::~DebugService()
{
}

void DebugService::setup(gameplay::SpriteBatch * whiteSprite, gameplay::Font * font, float scale)
{
    _whiteSprite = whiteSprite;
    _systemFont.reset(font);
    _fontSize = font->getSize() * scale;
}

bool DebugService::onPreInit()
{
    return true;
}

bool DebugService::onTick()
{
#ifndef __DISABLE_PROFILER__
    unsigned framesCount = gameplay::Game::getInstance( )->getFrameRate( );
    Profiler::Instance( ).saveProfilerResults( );
    Profiler::Instance( ).setFrameTime( _manager->getFrameTime() );
#endif
    return false;
}

bool DebugService::onInit()
{
    RenderService * rs = _manager->findService< RenderService >();

    _debugRenderStep.reset(rs->createRenderStep("DebugRenderStep"));

    _fpsRenderClick.reset(new MemFnRenderClick< DebugService >(this, &DebugService::drawFPS, "FPSRenderClick"));
    _debugRenderStep->addRenderClick(_fpsRenderClick.get());

    return true;
}

bool DebugService::onShutdown()
{
    _systemFont.reset();
    _fpsRenderClick.reset();
    _debugRenderStep.reset();
    return true;
}

void DebugService::drawTextOutline(float x, float y, const gameplay::Vector4& color, const wchar_t * text) const
{
    _systemFont->start();
    _systemFont->drawText(text, x - 1, y, gameplay::Vector4(0.0f, 0.0f, 0.0f, 1.0f), _fontSize);
    _systemFont->drawText(text, x + 1, y, gameplay::Vector4(0.0f, 0.0f, 0.0f, 1.0f), _fontSize);
    _systemFont->drawText(text, x, y - 1, gameplay::Vector4(0.0f, 0.0f, 0.0f, 1.0f), _fontSize);
    _systemFont->drawText(text, x, y + 1, gameplay::Vector4(0.0f, 0.0f, 0.0f, 1.0f), _fontSize);
    _systemFont->drawText(text, x, y, color, _fontSize);
    _systemFont->finish();
}

void DebugService::drawFPS() const
{
    drawTextOutline(1, 1, gameplay::Vector4::one(), Utils::format(L"FPS: {0, number, integer}", gameplay::Game::getInstance()->getFrameRate()));

#ifndef __DISABLE_PROFILER__
    if( _showProfiler )
        Profiler::Instance( ).drawPerformanceInfo( _systemFont, _whiteSprite, _fontSize * 0.5f );
#endif
}