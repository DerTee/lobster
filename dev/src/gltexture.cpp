// Copyright 2014 Wouter van Oortmerssen. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stdafx.h"

#include "glinterface.h"
#include "glincludes.h"

#include "stb_image.h"


uint CreateTexture(uchar *buf, const int2 &dim, int tf)
{
    uint id;
    glGenTextures(1, &id);
    assert(id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tf & TF_CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tf & TF_CLAMP ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tf & TF_NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tf & TF_NOMIPMAP ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);

    //if (mipmap) glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim.x(), dim.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);

    if (tf & TF_NOMIPMAP) return id;

    glEnable(GL_TEXTURE_2D);  // required on ATI for mipmapping to work?
    #if !defined(PLATFORM_MOBILE) && !defined(__APPLE__)
    if (glGenerateMipmap)     // only exists in 3.0 contexts and up.
    #endif
        glGenerateMipmap(GL_TEXTURE_2D);

    return id;
}

uint CreateTextureFromFile(const char *name, int2 &dim, int tf)
{
    size_t len = 0;
    auto fbuf = LoadFile(name, &len);
    if (!fbuf)
        return 0;

    int x, y, comp;
    auto buf = stbi_load_from_memory(fbuf, (int)len, &x, &y, &comp, 4);
    dim = int2(x, y);

    free(fbuf);

    if (!buf)
        return 0;

    uint id = CreateTexture(buf, dim, tf);

    stbi_image_free(buf);
    return id;
}

void DeleteTexture(uint id) { glDeleteTextures(1, &id); }

void SetTexture(uint textureunit, uint id)
{
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void SetImageTexture(uint textureunit, uint id, bool read)
{
    if (glBindImageTexture)
        glBindImageTexture(textureunit, id, 0, GL_FALSE, 0, read ? GL_READ_ONLY : GL_WRITE_ONLY, GL_RGBA8);
}

// from 2048 on older GLES2 devices and very old PCs to 16384 on the latest PC cards
int MaxTextureSize() { int mts = 0; glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mts); return mts; }


