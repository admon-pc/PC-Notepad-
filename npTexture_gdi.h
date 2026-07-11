#pragma once
#include "npTexture.h"



class npTexture_gdi :
    public npTexture
{
    friend class npRenderer_gdi;

    Gdiplus::Bitmap * m_bitmap = 0;
public:
    npTexture_gdi();
    virtual ~npTexture_gdi();


};

