#pragma once
#include <gfx_core.hpp>
#include <gfx_pixel.hpp>
#include <gfx_positioning.hpp>
#include "Adafruit_NeoPixel.h"
namespace arduino {

class w2812 {
public:
    using pixel_type = gfx::rgb_pixel<24>;
    using caps = gfx::gfx_caps<false,false,false,false,true,true,false>;
    Adafruit_NeoPixel m_np;
    int m_suspend_count;
    inline void update() {
        if(!m_suspend_count) {
            m_np.begin();
            m_np.show();
        }
    }
public:
    inline w2812(uint16_t count, uint8_t pin, neoPixelType type = NEO_GRB + NEO_KHZ800) : m_np(count,pin,type), m_suspend_count(0) {

    }
    inline bool initialize() {
        m_np.begin();
        return true;
    }
    inline gfx::size16 dimensions() const {
        return {m_np.numPixels(),1};
    }
    inline gfx::rect16 bounds() const {
        return dimensions().bounds();
    }
    inline gfx::gfx_result point(gfx::point16 location, pixel_type* out_color) const {
        if(out_color==nullptr) {
            return gfx::gfx_result::invalid_argument;
        }
        if(location.y!=0) {
            out_color->native_value = 0;
            return gfx::gfx_result::success;
        }
        const uint32_t r = m_np.getPixelColor(location.x);
        out_color->template channel<gfx::channel_name::R>(r&0xFF);
        out_color->template channel<gfx::channel_name::G>((r>>8)&0xFF);
        out_color->template channel<gfx::channel_name::B>((r>>16)&0xFF);
        return gfx::gfx_result::success;
    }
    inline gfx::gfx_result point(gfx::point16 location, pixel_type color) {
        m_np.begin();
        if(location.y!=0) {
            return gfx::gfx_result::success;
        }
        // TODO: can probably set this faster
        m_np.setPixelColor(location.x,color.template channel<gfx::channel_name::R>()|(color.template channel<gfx::channel_name::G>()<<8)|(color.template channel<gfx::channel_name::B>()<<16));
        update();
        return gfx::gfx_result::success;
    }
    inline gfx::gfx_result fill(const gfx::rect16& bounds, pixel_type color) {
        m_np.begin();
        const gfx::rect16 r = bounds.normalize();
        if(r.y1>0) {
            return gfx::gfx_result::success;
        }
        // TODO: can probably set this faster
        m_np.fill(r.width(),r.x1,color.template channel<gfx::channel_name::R>()|(color.template channel<gfx::channel_name::G>()<<8)|(color.template channel<gfx::channel_name::B>()<<16));
        update();
        return gfx::gfx_result::success;
    }
    inline gfx::gfx_result suspend() {
        ++m_suspend_count;
        return gfx::gfx_result::success;
    }
    inline gfx::gfx_result resume(bool force=false) {
        if(force) {
            m_suspend_count=1;
        }
        if(!--m_suspend_count) {
            update();
        }
        return gfx::gfx_result::success;
    }
};
}